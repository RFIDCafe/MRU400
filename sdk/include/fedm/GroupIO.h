/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_GROUP_IO_H
#define FEDM_GROUP_IO_H

#include <memory>
#include <vector>

#include "FedmBasic.h"
#include "FedmHide.h"



namespace FEDM {

/// @brief GROUP class to set reader outputs and to get reader inputs (commands [0x72], [0x74] and [0x2C])
///
/// All functions of the input queue are thread save
class FEDM_CLASS_DEF IO
{
	friend class ReaderModule;
	
public:
/** @name Main
 */
///@{			
	/// @brief Set reader outputs (command [0x72], modes [0x00] and [0x01])
	///
	/// @param[in] mode Mode 0x00 or 0x01 is valid here. See reader manual for details.
	/// @param[in] outputSettings A container with multiple output settings
	/// @return #{DOC_LIB_RETURN_VALUE}
	int setOutput(uint8_t mode, const std::vector<FEDM::OutputSetting> & outputSettings);

	/// @brief Set reader outputs (command [0x72], mode [0x04])
	///
	/// @param[in] outputSettings A container with multiple output settings
	/// @return #{DOC_LIB_RETURN_VALUE}
	int setOutput(const std::vector<FEDM::OutputSetting_04> & outputSettings);

	/// @brief Set find reader mode (command [0x72], modes [0x10] and [0x11])
	/// @param[in] on Switch find LED's on (mode [0x11]) / off (mode [0x10])
	/// @return #{DOC_LIB_RETURN_VALUE}
	int findReader(bool on);

	/// @brief Get reader inputs (command [0x74])
	///
	/// @param[out] input Returns the binary state of the reader hardware inputs
	/// @return #{DOC_LIB_RETURN_VALUE}
	int getInput(uint32_t & input);
///@}



/** @name Input Queue
 */
///@{
	/// @brief Clear the input queue
	void clearInQueue(void);

	/// @brief Set the maximum input queue size
	/// @param[in] max Maximum size of the queue.<br>
	/// In case of a full queue: The eldest item is removed before the new item is pushed.
	void setInQueueMaxItemCount(size_t max);

	/// @brief Get the maximum input queue size
	/// @return The maximum number of items in the queue
	size_t inQueueMaxItemCount(void) const;

	/// @brief Current input queue size
	/// @return The number of available input values in the queue
	size_t inQueueItemCount(void) const;

	/// @brief Input value access
	/// @return The next input value or InvalidInput if the queue is empty
	std::unique_ptr<const FEDM::InputEventItem> popInItem(void);
///@}

protected:
	IO();
	~IO();

	Internal::ReaderBase  *m_rdrBase; // set by a friend

};

}; // end of namespace FEDM

#endif // FEDM_GROUP_IO_H
