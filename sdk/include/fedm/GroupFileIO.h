/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_GROUP_FILE_IO_H
#define FEDM_GROUP_FILE_IO_H

#include <vector>

#include "FedmBasic.h"
#include "FedmHide.h"



namespace FEDM {

/// @brief GROUP class to read file from reader (command [0x40]), write file to reader (command [0x41]) and delete file on reader side (command [0x42]).
///
/// Data files on reader side are addressed via file ID's (see the reader manual for supported file ID's).
class FEDM_CLASS_DEF FileIO
{
	friend class ReaderModule;
	
public:
	/// @brief Read file data from reader to byte array (command [0x40])
	/// @param[in] fileId The ID of the remote file (see the reader manual for supported file ID's).
	/// @param[out] fileData The content of transfered file data.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int readFile(uint32_t fileId, std::vector<uint8_t> & fileData);

	/// @brief Read file data from reader to local file (command [0x40])
	/// @param[in] fileId The ID of the remote file (see the reader manual for supported file ID's).
	/// @param[in] dirName The name of the local directory to store the data. The filename is generated automatically.
	/// @param[out] fileName The absolute path and name of the written file.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int readFile(uint32_t fileId, const std::string & dirName, std::string & fileName);

	/// @brief Write byte array data to reader (command [0x41])
	/// @param[in] fileId The ID of the remote file (see the reader manual for supported file ID's).
	/// @param[in] fileData Byte array to be transfered
	/// @return #{DOC_LIB_RETURN_VALUE}
	/// 
	/// Remark: Depending on the reader type and file ID the activation of the transfered file data
	/// could extend the default communication receive timeout.<br>
	/// This value is set via the Connector class at communication connect and can be changed
	/// temporarily with ReaderModule::setReceiveTimeout().<br>
	/// Refer to the reader manual for details.
	int writeFile(uint32_t fileId, const std::vector<uint8_t> & fileData);

	/// @brief Write data of local file to reader (command [0x41])
	/// @param[in] fileId The ID of the remote file (see the reader manual for supported file ID's).
	/// @param[in] fileName The name of the local file to be transfered
	/// @return #{DOC_LIB_RETURN_VALUE}
	/// 
	/// Remark: Depending on the reader type and file ID the activation of the transfered file data
	/// could extend the default communication receive timeout.<br>
	/// This value is set via the Connector class at communication connect and can be changed
	/// temporarily with ReaderModule::setReceiveTimeout().<br>
	/// Refer to the reader manual for details.
	int writeFile(uint32_t fileId, const std::string & fileName);

	/// @brief Delete remote file of the reader (command [0x42])
	/// @param[in] fileId The ID of the remote file (see the reader manual for supported file ID's).
	/// @return #{DOC_LIB_RETURN_VALUE}
	int deleteFile(uint32_t fileId);

protected:
	FileIO();
	~FileIO();

	Internal::ReaderBase  *m_rdrBase; // set by a friend

};

}; // end of namespace FEDM

#endif // FEDM_GROUP_FILE_IO_H
