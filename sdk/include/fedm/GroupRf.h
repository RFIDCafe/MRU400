/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_GROUP_RF_H
#define FEDM_GROUP_RF_H

#include "FedmBasic.h"
#include "FedmHide.h"



namespace FEDM {

/// @brief GROUP class to control readers RF output
class FEDM_CLASS_DEF Rf
{
	friend class ReaderModule;
	
public:
	/// @brief Perform a RF reset (command [0x69])
	/// @return #{DOC_LIB_RETURN_VALUE}
	int reset(void);

	/// @brief Switch RF output off (command [0x6A])
	/// @return #{DOC_LIB_RETURN_VALUE}
	int off(void);

	/// @brief Switch RF output on (command [0x6A])
	/// @param[in] antennaNumber The number of antenna / RF output to switch on (first antenna is 1)
	/// @param[in] maintainHostMode If true, Host Mode is maintained when the selected antenna is switched on
	/// @param[in] dcOn If true, the DC supply and RF power are switched on together
	/// @return #{DOC_LIB_RETURN_VALUE}
	/// 
	/// Remarks:<br>
	/// The parameter maintainHostMode is applicable for Auto Read Mode only and those readers with support for that operation modes.<br>
	/// The parameter dcOn is applicable for readers with support for that.<br>
	/// Refer to the reader manual for details. Unsupported parameter are ignored.
	int on(int antennaNumber, bool maintainHostMode = false, bool dcOn = false);

	/// @brief Read noise level values (command [0x6D])
	/// @param[out] noiseLevel The measured noise level values
	/// @return #{DOC_LIB_RETURN_VALUE}
	int getNoiseLevel(FEDM::NoiseLevel & noiseLevel);

	/// @brief Write the UHF region configuration (command [0x8F])
	/// @param[in] uhfRegion The region settings which will be send to the reader
	/// @return #{DOC_LIB_RETURN_VALUE}
	/// 
	/// Remarks:<br>
	/// Locked region settings can be unlocked via firmware upgrade only!<br>
	/// Contact the FEIG support for that.<br>
	/// This function does not support UHF region expert settings.
	int setUhfRegion(const FEDM::UhfRegionConfig & uhfRegion);

	/// @brief Read the UHF region configuration (command [0x8E])
	/// @param[out] uhfRegion The region settings read from the reader
	/// @return #{DOC_LIB_RETURN_VALUE}
	int getUhfRegion(FEDM::UhfRegionConfig & uhfRegion);

	/// @brief Lock reader's current UHF region configuration (command [0x8D])
	/// @return #{DOC_LIB_RETURN_VALUE}
	/// 
	/// Remarks:<br>
	/// Locked region settings can be unlocked via firmware upgrade only!<br>
	/// Contact the FEIG support for that.
	int lockCurrentUhfRegion(void);

protected:
	Rf();
	~Rf();

	Internal::ReaderBase  *m_rdrBase; // set by a friend

};

}; // end of namespace FEDM

#endif // FEDM_GROUP_RF_H
