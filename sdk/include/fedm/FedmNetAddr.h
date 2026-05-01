/**
*
* Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
*
* This software is the confidential and proprietary information of
* FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
* disclose such Confidential Information and shall use it only in
* accordance with the terms of the license agreement you entered
* into with FEIG ELECTRONIC GmbH.
*/

#ifndef FEDM_NET_ADDR_H
#define FEDM_NET_ADDR_H

#include "FedmBase.h"

#include <string>
#include <array>



namespace FEDM {

/// @brief DATA class for one MAC address
class FEDM_CLASS_DEF MacAddress
{
	friend class FedmFriend;

public:
	/// @brief Construct a MAC address object filled with zeros
	MacAddress();

	/// @brief Construct a MAC address object
	/// @param[in] addr A MAC address string. In case of an invalid MAC address string the object is filled with zeros.
	MacAddress(const std::string & addr);

	/// @return 6
	size_t size(void) const;

	/// @brief Set all address bytes to zero
	void clear(void);

	/// @brief Check for non zeros
	/// @return False if all address bytes are zero
	bool isValid(void) const;

	/// @brief Get a pointer to the address byte array
	uint8_t *data(void);

	/// @brief Get a const pointer to the address byte array
	const uint8_t *data(void) const;

	/// @brief Get a reference to an address byte by index
	/// @param[in] n The index of the address byte
	/// @return A reference to the address byte of given index
	uint8_t & operator[] (size_t n);

	/// @brief Get an address byte by index
	/// @param[in] n The index of the address byte
	/// @return The address byte of given index
	uint8_t operator[] (size_t n) const;

	FEDM::MacAddress & operator=(const FEDM::MacAddress & other);

	bool operator==(const FEDM::MacAddress & other) const;
	bool operator!=(const FEDM::MacAddress & other) const;

	/// @brief Convert the address array to a MAC address string ('a:b:c:d:e:f')
	std::string toString(void) const;

	/// @brief Convert a MAC address string to address array
	/// @param[in] addr A MAC address string
	/// @return True if addr is an valid MAC address, false otherwise.
	/// 
	/// If the function return false, the address array is filled with zeros.
	bool fromString(const std::string & addr);

private:
	std::array<uint8_t, 6> m_array;
};



/// @brief DATA class for one IPv4 address
///
/// The bytes are in network (big endian) byte order.
class FEDM_CLASS_DEF IpV4Address
{
	friend class FedmFriend;

public:
	/// @brief Construct an IPv4 object filled with zeros
	IpV4Address();

	/// @brief Construct an IPv4 object
	/// @param[in] addr An IPv4 address string. In case of an invalid IPv4 string the object is filled with zeros.
	IpV4Address(const std::string & addr);

	/// @return 4
	size_t size(void) const;

	/// @brief Set all address bytes to zero
	void clear(void);

	/// @brief Check for non zeros
	/// @return False if all address bytes are zero
	bool isValid(void) const;

	/// @brief Get a pointer to the address byte array
	uint8_t *data(void);

	/// @brief Get a const pointer to the address byte array
	const uint8_t *data(void) const;

	/// @brief Get a reference to an address byte by index
	/// @param[in] n The index of the address byte
	/// @return A reference to the address byte of given index
	uint8_t & operator[] (size_t n);

	/// @brief Get an address byte by index
	/// @param[in] n The index of the address byte
	/// @return The address byte of given index
	uint8_t operator[] (size_t n) const;

	FEDM::IpV4Address & operator=(const FEDM::IpV4Address & other);

	bool operator==(const FEDM::IpV4Address & other) const;
	bool operator!=(const FEDM::IpV4Address & other) const;

	/// @brief Convert the address array to an IPv4 string ('a.b.c.d')
	std::string toString(void) const;

	/// @brief Convert an IPv4 string to address array
	/// @param[in] addr An IPv4 address string
	/// @return True if addr is an valid IPv4 address, false otherwise.
	/// 
	/// If the function return false, the address array is filled with zeros.
	bool fromString(const std::string & addr);

	/// @brief Get the netmask length
	/// @return The netmask length
	/// 
	/// This function is useful only if this object holds a network mask.
	size_t maskLength(void) const;

	/// @brief Create an IPv4 network mask
	/// @param[in] maskLength The number of bits in the mask
	/// @return An IPv4 object holding a network mask of given length
	static FEDM::IpV4Address createNetMask(size_t maskLength);

	/// @brief Remove leading zeros in an address string to trim it to a valid IPv4 string
	/// @param[in] addr The address string to trim
	/// @return A valid IPv4 string or an empty string if addr cannot be trimmed
//	static std::string trimmedAddress(const std::string & addr);

private:
	std::array<uint8_t, 4> m_array;
};



/// @brief DATA class for one IPv6 address
///
/// The bytes are in network (big endian) byte order.
class FEDM_CLASS_DEF IpV6Address
{
	friend class FedmFriend;

public:
	/// @brief Construct an IPv6 object filled with zeros
	IpV6Address();

	/// @brief Construct an IPv6 object
	/// @param[in] addr An IPv6 address string. In case of an invalid IPv6 string the object is filled with zeros.
	IpV6Address(const std::string & addr);

	/// @return 16
	size_t size(void) const;

	/// @brief Set all address bytes to zero
	void clear(void);

	/// @brief Check for non zeros
	/// @return False if all address bytes are zero
	bool isValid(void) const;

	/// @brief Check for LLA
	/// @return True for FE80::/10
	bool isLocalLinkAddress(void) const;

	/// @brief Get a pointer to the address byte array
	uint8_t *data(void);

	/// @brief Get a const pointer to the address byte array
	const uint8_t *data(void) const;

	/// @brief Get a reference to an address byte by index
	/// @param[in] n The index of the address byte
	/// @return A reference to the address byte of given index
	uint8_t & operator[] (size_t n);

	/// @brief Get an address byte by index
	/// @param[in] n The index of the address byte
	/// @return The address byte of given index
	uint8_t operator[] (size_t n) const;

	FEDM::IpV6Address & operator=(const FEDM::IpV6Address & other);

	bool operator==(const FEDM::IpV6Address & other) const;
	bool operator!=(const FEDM::IpV6Address & other) const;

	/// @brief Convert the address array to an IPv6 string
	std::string toString(void) const;

	/// @brief Convert an IPv6 string to address array
	/// @param[in] addr An IPv6 address string
	/// @return True if addr is an valid IPv6 address, false otherwise.
	/// 
	/// If the function return false, the address array is filled with zeros.
	bool fromString(const std::string & addr);

	/// @brief Get the netmask length
	/// @return The netmask length
	/// 
	/// This function is useful only if this object holds a network mask.
	size_t maskLength(void) const;

	/// @brief Create an IPv6 network mask
	/// @param[in] maskLength The number of bits in the mask
	/// @return An IPv6 object holding a network mask of given length
	static FEDM::IpV6Address createNetMask(size_t maskLength);

private:
	std::array<uint8_t, 16> m_array;

/* Unicast addresses:
*
*  - Link Locale:
*    FE80::<a>:<b>:<c>:<d>/64
*      with <a>:<b>:<c>:<d> is the 64Bit-Interface-ID
*
*  - Unique Locale:
*    FD00::/8 + 40Bit-Global-ID + 16Bit-Subnet-ID + 64Bit-Interface-ID  // private administration
*    FC00::/8 + 40Bit-Global-ID + 16Bit-Subnet-ID + 64Bit-Interface-ID  // global administration
*
*  - Unique Global:
*    2000::/3 + 61Bit-Subnet-ID + 64Bit-Interface-ID
*/
};

}; // end of namespace FEDM

#endif // FEDM_NET_ADDR_H
