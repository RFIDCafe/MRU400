/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_USER_H
#define FEDM_USER_H

namespace FEDM {


/// @brief DATA class for a managed Reader User
class FEDM_CLASS_DEF User
{
	friend class Internal::ReaderBase;
	friend class Payload::PlBase;
	friend class FedmFriend;

public:
	static const int AuthLevelAdmin = 15;
	static const int AuthLevelUser1 = 1;
	static const int AuthLevelLlrp  = 2;

	static const uint32_t PermHostInterface = 0x00000001;
	static const uint32_t PermAirInterface  = 0x00000002;
	static const uint32_t PermApplication   = 0x00000004;
	static const uint32_t PermDevice        = 0x00000008;
	static const uint32_t PermProfiles      = 0x00000010;
	static const uint32_t PermAll           = 0x7FFFFFFF;

	static FEDM::User createAdmin(const std::string & username, const std::string & password)
	{
		FEDM::User user;
		user.m_username = username;
		user.m_password = password;
		user.m_authentLevel = AuthLevelAdmin;
		user.m_permissionFlags = PermAll;
		return user;
	}

	static FEDM::User createUser(const std::string & username, const std::string & password, uint32_t permissionFlags)
	{
		FEDM::User user;
		user.m_username = username;
		user.m_password = password;
		user.m_authentLevel = AuthLevelUser1;
		user.m_permissionFlags = permissionFlags;
		return user;
	}

	static FEDM::User createUserLlrp(const std::string & username, const std::string & password)
	{
		FEDM::User user;
		user.m_username = username;
		user.m_password = password;
		user.m_authentLevel = AuthLevelLlrp;
		user.m_permissionFlags = 0;
		return user;
	}

	bool isValid(void) const
	{
		return (!m_username.empty()) && (m_authentLevel > 0);
	}

	const std::string & username(void) const { return m_username; }
	const std::string & password(void) const { return m_password; }

	bool isAdmin(void) const { return m_authentLevel == AuthLevelAdmin; }
	int  authentLevel(void) const { return m_authentLevel; }

	uint32_t permissionFlags(void) const { return m_permissionFlags; }

	std::string toString(void) const
	{
		std::string str =
			"Username=" + m_username +
			":Password=xxx" + //m_password + 
			":AuthLevel=" + std::to_string(m_authentLevel) +
			":PermFlags=" + std::to_string(m_permissionFlags);
		return str;
	}

private:
	std::string m_username;
	std::string m_password;
	int      m_authentLevel = -1;
	uint32_t m_permissionFlags = 0;
};

} // end of namespace FEDM

#endif // FEDM_USER_H
