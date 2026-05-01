/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_GROUP_USER_MANAGEMENT_H
#define FEDM_GROUP_USER_MANAGEMENT_H

#include "FedmBasic.h"
#include "FedmHide.h"

namespace FEDM {

/// @brief GROUP class for readers user management
class FEDM_CLASS_DEF UserManagement
{
	friend class ReaderModule;

public:
	static const int InvalidPermFlags = 0x80000000;

	/// @brief A login to get access to protected reader functions and configuration parameters with [0xA1] Reader Login command.
	/// @param[in] username An ASCII string
	/// @param[in] password An ASCII string
	/// @param[in] forceLogin If true an user with same autentication level is auto logged out.
	/// @return #{DOC_LIB_RETURN_VALUE}<br>
	/// An user with lower autentication level is auto logged out anyway.
	int readerLogin(const std::string & username, const std::string & password, bool forceLogin = false);


	/// @brief A logout to close protected function and configuration access with [0xA1] Reader Login command.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int readerLogout(void);


	/// @brief Add a new user to readers user management with [0xA4] Set User command.
	/// @param[in] user The user data to be added.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int addUser(const FEDM::User & user);


	/// @brief Remove a user from readers user management with [0xA4] Set User command.
	/// @param[in] username The username to remove.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int removeUser(const std::string & username);


	/// @brief Change the password and/or permission flags of an user with [0xA4] Set User command.
	/// @param[in] username The username which attributes will be changed
	/// @param[in] password The new password for user.
	/// If empty, the change operation does not affect the password.
	/// @param[in] permissionFlags The new permmision flags (categories) for the user.
	/// In case of InvalidPermFlags, the change operation does not affect the permission flags.
	/// @return #{DOC_LIB_RETURN_VALUE}
	int changeUser(const std::string & username, const std::string & password, uint32_t permissionFlags = FEDM::UserManagement::InvalidPermFlags);


	/// @brief Get the list of readers managed users with [0xA5] Get Users command.
	/// @param[out] userList A vector of available users
	/// @return #{DOC_LIB_RETURN_VALUE}
	int getUsers(std::vector<FEDM::User> & userList);


protected:
	UserManagement();
	~UserManagement();

	Internal::ReaderBase *m_rdrBase; // set by a friend

};

} // end of namespace FEDM

#endif // FEDM_GROUP_USER_MANAGEMENT_H
