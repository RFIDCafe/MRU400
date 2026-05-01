/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_CONNECTOR_H
#define FEDM_CONNECTOR_H

#include <string>

#include "FedmConst.h"



namespace FEDM {

/// @brief DATA class with connection parameter of a reader
class FEDM_CLASS_DEF Connector
{
	friend class FedmFriend;

public:
	static const int ConnectNone = 0; ///< Connection type invalid
	static const int ConnectCom = 1;  ///< Connection type serial COM
	static const int ConnectUsb = 2;  ///< Connection type USB
	static const int ConnectTcp = 3;  ///< Connection type TCP
	static const int ConnectTls = 4;  ///< Connection type TLS
	static const int ConnectBthLe = 5;   ///< Connection type Bluetooth Low Energy
	static const int ConnectBthSpp = 6;  ///< Connection type Bluetooth SPP (Serial Port Profile))

public:
/** @name Main
 */
///@{
	/// @brief Create an invalid connector
	///
	/// Use the various set-functions to build an valid connector
	Connector(void);

	/// @brief Destructor
	~Connector(void);

	/// @brief Set the connector to state invalid
	void clear(void);

	/// @brief Check the validity of the connector
	bool isValid(void) const { return m_connectType != ConnectNone; }

	/// @brief Get the connection type
	int connectType(void) const { return m_connectType; }
///@}

/** @name Serial COM Parameter
 */
///@{
	/// @brief Create a valid connector ready for a serial COM connect
	///
	/// @param[in] port The communication port:<br>
	/// Under Windows port is something like 'COM1', 'com1', '1' <br>
	/// Under Linux port is something like '/dev/ttyS0'
	/// @param[in] frame Is '8E1' or '8N1' or '8O1'
	/// @param[in] baudrate Valid values:<br>
	/// 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
	/// @param[in] busAddr The bus address for RS485
	/// @return A serial COM connector
	static FEDM::Connector createComConnector(const std::string & port, const std::string & frame = "8E1", int baudrate = 38400, int busAddr = 255);

	/// @brief Set this connector to a serial COM type
	///
	/// @param[in] port The communication port:<br>
	/// Under Windows port is something like 'COM1', 'com1', '1' <br>
	/// Under Linux port is something like '/dev/ttyS0'
	/// @param[in] frame Is '8E1' or '8N1' or '8O1'
	/// @param[in] baudrate Valid values:<br>
	/// 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
	/// @param[in] busAddr The bus address for RS485
	void setCom(const std::string & port, const std::string & frame = "8E1", int baudrate = 38400, int busAddr = 255);

	/// @brief Set the bus address for RS485
	///
	/// The connection type should be set to serial COM before, otherwise this call will have no effect.
	void setComBusAddress(int busAddr) { m_comBusAddress = busAddr; }

	std::string comPort      (void) const { return m_comPort; }
	std::string comFrame     (void) const { return m_comFrame; }
	int         comBaudrate  (void) const { return m_comBaudrate; }
	int         comBusAddress(void) const { return m_comBusAddress; }
///@}

/** @name USB Parameter
 */
///@{					
	/// @brief Create a valid connector ready for a USB connect
	///
	/// @param[in] deviceId The device ID of the reader
	/// @return An USB connector
	static FEDM::Connector createUsbConnector(uint32_t deviceId, bool useWinUsb = false);

	/// @brief Set this connector to USB type
	void setUsb(uint32_t deviceId, bool useWinUsb = false);

	/// @brief Set this connector to USB type
	void setUsb(const std::string & hexDeviceId, bool useWinUsb = false);

	uint32_t    usbDeviceId(void) const { return m_usbDeviceId; }
	std::string usbDeviceIdToHexString(void) const;
///@}

/** @name TCP Parameter
 */
///@{
	/// @brief Create a valid connector ready for a TCP connect
	///
	/// @param[in] ipAddr Can be an IpV4 / IPv6 address or a fully qualified domain name (FQDN): "<hostname>.<domain>"
	/// @param[in] port The port number
	/// @return A TCP connector
	static FEDM::Connector createTcpConnector(const std::string & ipAddr, int port = 10001);

	/// @brief Set this connector to TCP type
	void setTcp(const std::string & ipAddr, int port = 10001);

	/// @brief Set the timeout for a TCP connect
	/// @param[in] msec The maximum wait time for a TCP connect in milliseconds.<br>
	/// For values <= 0 (the default) a blocking connect is performed. Otherwise a non-blocking connect is performed with the given wait time.
	///
	/// The particular behavior of a TCP connect depends on the operating system.
	void setTcpConnectTimeout(int msec) { m_tcpConnectTimeout = msec; }

	std::string tcpIpAddress(void) const { return m_tcpIpAddress; }
	int         tcpPort     (void) const { return m_tcpPort; }
	int         tcpConnectTimeout(void) const { return m_tcpConnectTimeout; }
///@}

/** @name TLS Parameter
 */
///@{
	/// @brief Create a valid connector ready for a TLS connect
	///
	/// @param[in] ipAddr Can be an IpV4 / IPv6 address or a fully qualified domain name (FQDN): "<hostname>.<domain>"
	/// @param[in] port The port number
	/// @return A TLS connector
	static FEDM::Connector createTlsConnector(const std::string & ipAddr, int port = 10001);

	/// @brief Set this connector to TLS type
	void setTls(const std::string & ipAddr, int port = 10001);

	/// @brief Set the timeout for a TLS connect
	/// @param[in] msec The maximum wait time for a TLS connect in milliseconds.<br>
	/// For values <= 0 (the default) a blocking connect is performed. Otherwise a non-blocking connect is performed with the given wait time.
	///
	/// The particular behavior of a TCP connect depends on the operating system.
	void setTlsConnectTimeout(int msec) { m_tcpConnectTimeout = msec; }

	/// @brief Set the identity to lookup for a PSK
	/// @param[in] identity The search string for a PSK
	void setTlsPskIdentity(const std::string & identity) { m_tlsPskIdentity = identity; }

	std::string tlsIpAddress(void) const { return m_tcpIpAddress; }
	int         tlsPort     (void) const { return m_tcpPort; }
	int         tlsConnectTimeout(void) const { return m_tcpConnectTimeout; }
	std::string tlsPskIdentity(void) const { return m_tlsPskIdentity; }
///@}

/** @name Bluetooth Low Energy Parameter
 */
///@{
	/// @brief Create a valid connector ready for a BLE connect
	///
	/// @param[in] macAddr The MAC address of the bluetooth device (e.g. 01:AB:23:CD:45:EF)
	/// @return A Bluetooth LE connector
	static FEDM::Connector createBluetoothLeConnector(const std::string & macAddr);

	/// @brief Set this connector to BthLe type
	void setBluetoothLe(const std::string & macAddr);

	std::string bluetoothLeMacAddress(void) const { return m_bluetoothLeMacAddress; }
///@}
	
/** @name Bluetooth SPP (Serial Port Profile) Parameter - Android only
 */
///@{
	/// @brief Create a valid connector ready for a Bluetooth SPP connect
	///
	/// @param[in] macAddr The MAC address of the bluetooth device (e.g. 01:AB:23:CD:45:EF)
	/// @return A Bluetooth SPP connector
	static FEDM::Connector createBluetoothSppConnector(const std::string & macAddr, bool secure = true);

	/// @brief Set this connector to BthSpp type
	void setBluetoothSpp(const std::string & macAddr, bool secure = true);

	std::string bluetoothSppMacAddress(void) const { return m_bluetoothSppMacAddress; }
	bool isBluetoothSppSecure(void) const { return m_bluetoothSppSecure; }
///@}

/** @name Communication Parameter
 */
///@{
	/// @brief Set delay between connect and first request command
	/// @param[in] msec Delay time between connect and first request command in milliseconds.<br>
	/// The default value is 0 msec
	void setPostConnectDelay(int msec) { m_postConnectDelay = msec; }

	/// @brief Get delay between connect and first request command
	int postConnectDelay(void) const { return m_postConnectDelay; }

	/// @brief Set the receive timeout value
	/// @param[in] msec The maximum wait time between request and response in milliseconds.<br>
	/// The default value is 5000 msec
	void setReceiveTimeout(int msec) { m_receiveTimeout = msec; }

	/// @brief Get the receive timeout value
	int receiveTimeout(void) const { return m_receiveTimeout; }

	/// @brief Set the auto reader information policy after connect
	/// @param[in] infoPolicy See ReaderInfoPolicy for details
	void setReaderInfoPolicy(FEDM::ReaderInfoPolicy infoPolicy) { m_readerInfoPolicy = infoPolicy; }

	/// @brief Get the auto reader information policy after connect
	///
	/// See ReaderInfoPolicy for details
	/// Default is ReaderInfoPolicy::All
	FEDM::ReaderInfoPolicy readerInfoPolicy(void) const { return m_readerInfoPolicy; }

	/// @brief Set username and password for automatic login after connect
	///
	/// An automatic login after connect is performed if both strings are not empty.
	void setAuthentication(const std::string & username, const std::string & password);

	/// @brief Query if authentication data is set.
	///
	/// @return True if username and password are not empty.
	bool isAuthentication(void) const;

	std::string authentUsername(void) const { return m_username; }
	std::string authentPassword(void) const { return m_password; }

///@}

/** @name Convert Current Connector Parameter (to and from FEISP Format)
 */
///@{					
	std::string connectorToString(void) const;
	void        connectorFromString(const std::string & connector);
///@}

private:
	int         m_connectType;

	// Serial port
	std::string m_comPort;
	std::string m_comFrame;
	int         m_comBaudrate;
	int         m_comBusAddress;

	// USB
    uint32_t    m_usbUseFlags;
	uint32_t    m_usbDeviceId;

	// TCP / TLS
	std::string m_tcpIpAddress;
	int         m_tcpPort;
	int         m_tcpConnectTimeout;
	std::string m_tlsPskIdentity;

	// BTH-LE
	std::string m_bluetoothLeMacAddress;

	// BTH-SPP
	std::string m_bluetoothSppMacAddress;
	bool        m_bluetoothSppSecure;

	// Misc
	int         m_postConnectDelay;
	int         m_receiveTimeout;
	FEDM::ReaderInfoPolicy m_readerInfoPolicy;
	std::string m_username;
	std::string m_password;

	void clearAuthent(void);

	void setUsbUseFlag(bool useWinUsb);
}; // end of class Connector



/// @brief DATA class with parameter for serial detection
class FEDM_CLASS_DEF DetectorSerial
{
	friend class FedmFriend;

public:
	static const int ModeSlow = 0x00; ///< All baudrates and byte frames will be checked (7 x 3 combinations)
	static const int ModeFast = 0x01; ///< Only baudrates 38400, 57600, 115200 will be checked (3 x 3 combinations)

/** @name Main
 */
///@{
	/// @brief Create an invalid detector
	DetectorSerial();

	/// @brief Destructor
	~DetectorSerial();

	/// @brief Set the detector invalid
	void clear(void);

	/// @brief Check the validity of the detector
	bool isValid(void) const { return !m_comPort.empty(); }

	/// @brief Create a valid detector
	/// @param[in] comPort The COM port where the detection shall take place<br>
	/// Under Windows port is something like 'COM1', 'com1', '1' <br>
	/// Under Linux port is something like '/dev/ttyS0' <br>
	/// @param[in] mode The detection mode
	static FEDM::DetectorSerial createDetector(const std::string & comPort, int mode = FEDM::DetectorSerial::ModeSlow);

	/// @brief Set the detection COM port
	void setComPort(const std::string & comPort) { m_comPort = comPort; }

	/// @brief Set the bus address for RS485
	void setBusAddress(int busAddress) { m_busAddress = busAddress; }

	/// @brief Set the detection mode
	///
	/// Default is ModeSlow
	void setMode(int mode) { m_mode = mode; }

	/// @brief Set the detection delay between consecutive trials
	///
	/// Each trial needs ca. 300 msec plus delay
	/// The default delay is 5 msec
	void setDelay(int msec) { m_delay = msec; }

	std::string comPort   (void) const { return m_comPort; }
	int         busAddress(void) const { return m_busAddress; }
	int         mode      (void) const { return m_mode; }
	int         delay     (void) const { return m_delay; }
///@}

/** @name Communication Parameter
 */
///@{
	/// @brief Set the auto reader information policy after detect
	/// @param[in] infoPolicy See ReaderInfoPolicy for details
	void setReaderInfoPolicy(FEDM::ReaderInfoPolicy infoPolicy) { m_readerInfoPolicy = infoPolicy; }

	/// @brief Get the auto reader information policy after detect
	///
	/// See ReaderInfoPolicy for details
	/// Default is ReaderInfoPolicy::All
	FEDM::ReaderInfoPolicy readerInfoPolicy(void) const { return m_readerInfoPolicy; }
///@}

/** @name Convert Current Connector Parameter (to FEISP Format)
 */
///@{					
	std::string toString(void) const;
///@}

private:
	std::string m_comPort;
	int         m_busAddress;
	int         m_mode;
	int         m_delay;
	FEDM::ReaderInfoPolicy m_readerInfoPolicy;
}; // end of class DetectorSerial



/// @brief DATA class with parameter of a listener thread waiting for reader connection requests
class FEDM_CLASS_DEF ListenerParam
{
	friend class FedmFriend;

public:
	static const int ListenNone = 0; ///< Listener type invalid
	static const int ListenTcp = 1;  ///< Listener type TCP
	static const int ListenTls = 2;  ///< Listener type TLS

/** @name Main
 */
///@{					
	/// @brief Create an invalid listener parameter object
	ListenerParam();

	/// @brief Destructor
	~ListenerParam();

	/// @brief Set the listener parameter invalid
	void clear(void);

	/// @brief Check the validity of the listener parameter
	bool isValid(void) const { return m_listenType != ListenNone; }

	/// @brief Get the listener type
	int  listenType(void) const { return m_listenType; }
///@}

/** @name TCP Listen Parameter
 */
///@{
	/// @brief Create valid listener parameter ready for TCP connection requests
	/// @param[in] port TCP port on which to listen to
	/// @param[in] ipAddr IP address of the network interface to listen to (Default: any IPv4)
	/// @param[in] keepAlive If true keep alive packets will be send
	/// @return A TCP listener parameter object
	static FEDM::ListenerParam createTcpListenerParam(int port, const std::string & ipAddr = "0.0.0.0", bool keepAlive = true);

	/// @brief Set to TCP listener parameter
	void setTcp(int port, const std::string & ipAddr = "0.0.0.0", bool keepAlive = true);

	/// @brief Set keep alive parameter
	/// @param[in] idle See socket documentation (default 1 sec)
	/// @param[in] count See socket documentation (default 5)
	/// @param[in] interval See socket documentation (default 1 sec)
	void setTcpKeepAlive(int idle, int count, int interval);

	int  tcpPort        (void) const { return m_tcpPort; }
	bool isTcpKeepAlive (void) const { return m_isTcpKeepAlive; }
	int  tcpKeepIdle    (void) const { return m_tcpKeepIdle; }
	int  tcpKeepCount   (void) const { return m_tcpKeepCount; }
	int  tcpKeepInterval(void) const { return m_tcpKeepInterval; }

	static std::string tcpAnyIPv4(void) { return "0.0.0.0"; }
	static std::string tcpAnyIPv6(void) { return "::"; }
///@}

/** @name TLS Listen Parameter
 */
///@{
	/// @brief Create valid listener parameter ready for TLS connection requests
	/// @param[in] port TCP port on which to listen to
	/// @param[in] keepAlive If true keep alive packets will be send
	/// @param[in] ipAddr IP address of the network interface to listen to (Default: any IPv4)
	/// @return A TCP listener parameter object
	static FEDM::ListenerParam createTlsListenerParam(int port, const std::string & ipAddr = "0.0.0.0", bool keepAlive = true);

	/// @brief Set to TLS listener parameter
	void setTls(int port, const std::string & ipAddr = "0.0.0.0", bool keepAlive = true);

	/// @brief Set the identity to lookup for a PSK
	/// @param[in] identity The search string for a PSK
	void setTlsPskIdentity(const std::string & identity) { m_tlsPskIdentity = identity; }

	std::string tlsPskIdentity(void) const { return m_tlsPskIdentity; }
///@}

	std::string toString(void) const; ///< Convert Current ListenerParam (to FEISP Format)

private:
	int         m_listenType;

	// TCP / TLS
	std::string m_tcpIpAddress;
	int         m_tcpPort;
	bool        m_isTcpKeepAlive;  // default true
	int         m_tcpKeepIdle;     // default 1 sec
	int         m_tcpKeepCount;    // default 5
	int         m_tcpKeepInterval; // default 1 sec
	std::string m_tlsPskIdentity;
}; // end of class ListenerParam

}; // end of namespace FEDM

#endif // FEDM_CONNECTOR_H
