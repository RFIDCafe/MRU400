/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_TLS_MANAGER_H
#define FEDM_TLS_MANAGER_H

#include <string>

#include "FedmBasic.h"



namespace FEDM {

/// @brief MANAGER class for TLS configuration
///
/// Usage of Certificates:<br>
/// <br>
/// Application request and reader response (HM, BRM):<br>
/// The application acts as client, i.e. performs the connect to start communication. The reader acts as server.<br>
/// Refer to FEDM::ReaderModule::connect() and class FEDM::Connector for how to establish a TLS connection.<br>
/// Use TlsManager::setClientVerifyCaCertificateChainFile() to set the applications client verify CA certificate chain.<br>
/// The readers server certificate chain and server private key must be send via FEDM::FileIO::writeFile().<br>
/// File IDs: FEDM::FileId::TlsSrvCertChain and FEDM::FileId::TlsSrvPrivKey.<br>
/// <br>
/// Reader request and application response (NTFM):<br>
/// The application acts as server, i.e. starts a listener to wait for communication requests. The reader acts as client.<br>
/// Refer to classes FEDM::Async and FEDM::ListenerParam for how to start a TLS listener.<br>
/// At application side we need the server certificate chain, the server private key and the server verify CA certificate chain.<br>
/// Use TlsManager::setServerCertificateChainFile(), TlsManager::setServerPrivateKeyFile() and TlsManager::setServerVerifyCaCertificateChainFile()
/// to configure these files.<br>
/// At reader side we need the client certificate chain, the client private key and the client verify CA certificate chain.
/// These files must be send via FEDM::FileIO::writeFile().<br>
/// File IDs: FEDM::FileId::TlsCliCertChain, FEDM::FileId::TlsCliPrivKey and FEDM::FileId::TlsCliCaCertChain.<br>
/// <br>
/// Remarks:<br>
/// To upload a [certificate/private-key] pair send the certificate chain file first and the private key file second.<br>
/// Ensure the reader has a valid system time set to keep the certificates valid. Register a NTP server address in the reader configuration.<br>
/// In a productive software the client / server certificates and keys must be different for each request / response channel.<br>
/// The client / server verify CA certificate chain at application side can be identical.<br>
/// All key and certificate files must be in PEM format.<br>
/// <br><br>
/// Usage of PSK's (Pre-shared Keys):<br>
/// <br>
/// Each reader must obtain an individual PSK, which is used for all operation modes (HM, BRM, NTFM).<br>
/// Refer to class FEDM::TlsPsk and FEDM::FileIO::writeFile() how to establish a reader PSK.<br>
/// At application side all PSK's must be stored in a PSK lookup table (PSK set).<br>
/// The TlsManager supports different PSK sets for client (HM, BRM) and server (NTFM) host apps.<br>
/// Use addClientPsk() and addServerPsk() to configure the lookup tables. Both tables can be identical.<br>
/// <br>
/// To establish a TLS PSK connection the PSK identity is used in addition to IP address and port.<br>
/// Refer to FEDM::ReaderModule::connect() and class FEDM::Connector for how to establish a TLS connection using a PSK client identity (HM, BRM).<br>
/// Refer to classes FEDM::Async and FEDM::ListenerParam for how to start a TLS listener using a PSK server identity (NTFM).<br>
/// <br>
/// Remark:<br>
/// Each listener will accept one reader connection request only. Therefore a listener instance for each reader must be present.<br>
/// In case of an application for exclusive NTFM usage all PSK's could be identical.<br>
/// Refer to RFC 9257 for more details to PSK usage.<br>
/// <br><br>
/// The TLS communication is supported by FEIG fetls library and openssl (i.e. libssl and libcrypto).<br>
/// We recommend openssl version 3.2 and higher.
class FEDM_CLASS_DEF TlsManager
{
public:
/** @name Main
 */
///@{					

	/// @brief Set TLS versions for server and client
	/// @param[in] minVersion The minimum TLS version to support
	/// @param[in] maxVersion The maximum TLS version to support
	/// @return #{DOC_LIB_ERROR_VALUE}
	///
	/// Valid values are 'TlsV1.2' and 'TlsV1.3'. TLS version 1.1 is not supported by fetls.
	static int setTlsVersions(const std::string & minVersion, const std::string & maxVersion);

	/// @brief Set TLSv1.2 cipher list for server and client
	/// @param[in] cipherList A cipher filter to define a list of cipher suites.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_set_cipher_list().<br>
	/// If cipherList is empty the default value obtained by OSSL_default_cipher_list() is used.<br>
	/// Refer to the openssl documentation for details of the filter syntax (and see command "openssl -ciphers -v -cipher ...").<br>
	/// Examples:<br>
	/// <br>
	/// TLSv1.2+ECDSA<br>
	/// For certificate based cipher suites using ECDSA authentication<br>
	/// <br>
	/// TLSv1.2+PSK+AES256:TLSv1.2+PSK+ARIA256:TLSv1.2+PSK+CHACHA20<br>
	/// For PSK with 256 bit length<br>
	/// <br>
	/// TLSv1.2+PSK+AES128:TLSv1.2+PSK+ARIA128<br>
	/// For PSK with 128 bit length<br>
	static int setTls12CipherList(const std::string & cipherList);

	/// @brief Set TLSv1.3 cipher suites for server and client
	/// @param[in] cipherSuites A list of colon seperated cipher suites.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_set_ciphersuites().<br>
	/// If cipherSuites is empty the default value obtained by OSSL_default_ciphersuites() is used.<br>
	/// Refer to the openssl documentation for details.
	static int setTls13CipherSuites(const std::string & cipherSuites);

	static int lastError(void);
	static std::string lastErrorText(void);

///@}



// #################################################### USAGE OF CERTIFICATES ##########################################
/** @name Usage of Certificates
 */
///@{

	/// @brief Set applications server private key (NTFM).
	/// @param[in] filename The private key filename (PEM format)
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_use_PrivateKey_file()
	static int setServerPrivateKeyFile(const std::string & filename);

	/// @brief Set applications server certificate chain (NTFM).
	/// @param[in] filename The certificate chain filename (PEM format)
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_use_certificate_chain_file().<br>
	static int setServerCertificateChainFile(const std::string & filename);

	/// @brief Set applications server certificate chain (NTFM).
	/// @param[in] filename The certificate chain filename (PEM format)
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_load_verify_locations().<br>
	/// <br>
	/// Remark: In most cases the same file is used for setClientVerifyCaCertificateChainFile()
	static int setServerVerifyCaCertificateChainFile(const std::string & filename);

	/// @brief Set applications client certificate chain (HM, BRM).
	/// @param[in] filename The certificate chain filename (PEM format)
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This function uses the openssl function SSL_CTX_load_verify_locations().<br>
	/// 
	/// Remark: In most cases the same file is used for setServerVerifyCaCertificateChainFile()
	static int setClientVerifyCaCertificateChainFile(const std::string & filename);

	/// @brief Check servers private key against certificate chain
	/// @param[out] result A result text of the verification
	/// @return #{DOC_LIB_ERROR_VALUE}
	static int checkServerPrivateKey(std::string & result);

	/// @brief Check the validity of a certificate file against local system time
	/// @param[out] result A result text of the verification
	/// @return #{DOC_LIB_ERROR_VALUE}
	static int checkCertificateFile(const std::string & filename, std::string & result);

///@}



// #################################################### USAGE OF PSK'S ##########################################
/** @name Usage of PSK's
 */
///@{

	/// @brief Add PSK to the internal server and client PSK lookup table.
	/// @param[in] psk The PSK object to add.
	/// 
	/// This is a convenience function to call addServerPsk() and addClientPsk().
	static int addPsk(const FEDM::TlsPsk & psk);

	/// @brief Add PSK to the internal server PSK lookup table.
	/// @param[in] psk The PSK object to add.
	static int addServerPsk(const FEDM::TlsPsk & psk);
	
	/// @brief Add PSK to the internal client PSK lookup table.
	/// @param[in] psk The PSK object to add.
	static int addClientPsk(const FEDM::TlsPsk & psk);

	/// @brief Clear the internal server and client PSK lookup table.
	static void clearPskLookupTables(void);

	/// @brief Load applications server PSK lookup table from file (NTFM).
	/// @param[in] filename The PSK-set file.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This file is encryted. Use setPskCryptoPassword() to set the crypto password for decryption before.
	static int loadServerPskSetFile(const std::string & filename);

	/// @brief Load applications client PSK lookup table from file (HM, BRM).
	/// @param[in] filename The PSK-set file.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This file is encryted. Use setPskCryptoPassword() to set the crypto password for decryption before.
	static int loadClientPskSetFile(const std::string & filename);

	/// @brief Save applications server PSK lookup table to file (NTFM).
	/// @param[in] filename The PSK-set file.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This file will be encryted. Use setPskCryptoPassword() to set the crypto password for encryption before.
	static int saveServerPskSetFile(const std::string & filename);

	/// @brief Save applications client PSK lookup table to file (HM, BRM).
	/// @param[in] filename The PSK-set file.
	/// @return #{DOC_LIB_ERROR_VALUE}
	/// 
	/// This file will be encryted. Use setPskCryptoPassword() to set the crypto password for encryption before.
	static int saveClientPskSetFile(const std::string & filename);

	/// @brief Set the crypto password for PSK-set files.
	/// @param[in] password The password for file en/decryption
	static int setPskCryptoPassword(const std::string & password);

///@}



// #################################################### DEVELOPEMENT ##########################################
/** @name Developement
 */
///@{

	/// @brief Load entire TLS configuration from file
	/// @param[in] filename The configuration filename.
	/// @return #{DOC_LIB_ERROR_VALUE}
	///
	/// This function loads all specified certificates. Use it for development purposes.<br>
	/// If the file does not exist a template with default values is generated.
	static int loadTlsConfiguration(const std::string & filename);

///@}

//	static int setPassPhrase(const std::string & password); // Do we need that ?

}; // end of class TlsManger

}; // end of namespace FEDM

#endif // FEDM_TLS_MANAGER_H
