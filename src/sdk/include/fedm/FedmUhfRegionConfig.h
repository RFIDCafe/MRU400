/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_UHF_REGION_CONFIG_H
#define FEDM_UHF_REGION_CONFIG_H

#include <FedmBase.h>



namespace FEDM {

/// @brief DATA class with UHF region configuration
class FEDM_CLASS_DEF UhfRegionConfig
{
	friend class Payload::PlBase;
	friend class FedmFriend;

public:
	void clear(void) { *this = FEDM::UhfRegionConfig(); }

	bool isValid(void) const { return m_uhfRegion != FEDM::UhfRegion::Invalid; }
	FEDM::UhfRegion uhfRegion(void) const { return m_uhfRegion; }
	void setUhfRegion(UhfRegion uhfRegion) { m_uhfRegion = uhfRegion; }

	bool isRegionLocked(void) const { return m_isRegionLocked; }
	void lockRegion(void) { m_isRegionLocked = true; }

	bool isExpert(void) const { return m_expertMode >= 0; }
	int  expertMode(void) const { return m_expertMode; }

	const std::vector<uint8_t> & channels(void) const { return m_expertChannels; }

	/// @brief Create an UHF region standard configuration
	static FEDM::UhfRegionConfig create(FEDM::UhfRegion uhfRegion, bool lockRegion = false)
	{
		FEDM::UhfRegionConfig cfg;
		cfg.setUhfRegion(uhfRegion);
		if (lockRegion) { cfg.lockRegion(); }
		return cfg;
	}

private:
	UhfRegion m_uhfRegion = UhfRegion::Invalid;
	bool      m_isRegionLocked = false;
	int       m_expertMode = -1;
	std::vector<uint8_t> m_expertChannels;

}; // end of class UhfRegionConfig

}; // end of namespace FEDM

#endif // FEDM_UHF_REGION_CONFIG_H
