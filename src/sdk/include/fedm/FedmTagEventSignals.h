/*
 * Copyright (C) FEIG ELECTRONIC GmbH. All rights reserved. Do not disclose.
 *
 * This software is the confidential and proprietary information of
 * FEIG ELECTRONIC GmbH ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with FEIG ELECTRONIC GmbH.
 */

#ifndef FEDM_TAG_EVENT_SIGNALS_H
#define FEDM_TAG_EVENT_SIGNALS_H

#include <FedmBase.h>



namespace FEDM {

/// @brief DATA class with signals for TagEventItem
///
/// Signal flags are active high.
class FEDM_CLASS_DEF TagEventSignals
{
	friend class FedmFriend;

public:
	TagEventSignals() { clear(); }

	void clear(void) { m_signalFlags = 0; }
	bool isEmpty(void) const { return m_signalFlags == 0; } // isValid() means !isEmpty()

	bool isEasAlarm (void) const { return isBit(m_signalFlags, 0); }
	bool isRfWarnOn (void) const { return isBit(m_signalFlags, 1); }
	bool isRfWarnOff(void) const { return isBit(m_signalFlags, 2); }

	void setEasAlarm (bool on) { setBit(m_signalFlags, 0, on); }
	void setRfWarnOn (bool on) { setBit(m_signalFlags, 1, on); }
	void setRfWarnOff(bool on) { setBit(m_signalFlags, 2, on); }

	uint32_t signalFlags(void) const { return m_signalFlags; }
	void setSignalFlags(uint32_t signalFlags) { m_signalFlags = signalFlags; }

	std::string toString(void) const
	{
		std::string str;
		if (isEasAlarm())  { str += "EASalarm-"; }
		if (isRfWarnOn())  { str += "RFwarn-"; }
		if (isRfWarnOff()) { str += "RFok-"; }
		str.pop_back();
		return str;
	}

private:
    uint32_t   m_signalFlags;

	static bool isBit(uint32_t signalFlags, int bit)
	{
		uint32_t mask = 0x00000001;
		mask <<= bit;
		return (signalFlags & mask) != 0;
	}

	static void setBit(uint32_t & signalFlags, int bit, bool on)
	{
		uint32_t mask = 0x00000001;
		mask <<= bit;
		if (on)
			signalFlags |= mask;
		else
			signalFlags &= ~mask;
	}

}; // end of class TagEventSignals

}; // end of namespace FEDM

#endif // FEDM_TAG_EVENT_SIGNALS_H
