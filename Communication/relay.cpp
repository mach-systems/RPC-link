/*
 * relay.cpp
 *
 *  Created on: Mar 4, 2025
 *      Author: karel
 */

#include <iostream>
#include "autotalksLink.hpp"
#include "v2xUtils.hpp"
#include "relay.hpp"


#define TX_BUFFER_SIZE      2048


Relay::Relay(AutotalksLink& V2XLink, bool physPrint)
             : m_V2XLink(V2XLink),
			   m_PhysPrint(physPrint)
{
    m_V2XLink.SetNetIndication(std::bind(&Relay::PacketReceived, this, std::placeholders::_1));
    m_V2XLink.SetNetDcc(std::bind(&Relay::DccMeasuredCallback, this, std::placeholders::_1));
}

void Relay::DccMeasuredCallback(double cbr)
{
    if (m_NetDccTransmit)
    {
        ChannelBusyRatio channelBusyRatio;
        channelBusyRatio.Busy = (uint16_t) cbr * 100;
        channelBusyRatio.Samples = 100;
        m_NetDccTransmit(channelBusyRatio);
    }
}

void Relay::PacketSend(V2xFrame& frame) const
{
	if (m_PhysPrint)
	{
		std::cout << "V2X transmitting (networking layer): " << std::endl;
		hexDump(frame.Data);
	}
	m_V2XLink.request(frame);
}

void Relay::PacketReceived(const V2xFrame& frame)
{
    if (m_NetTransmit)
    {
        m_NetTransmit(frame);
    }
}

void Relay::SetNetIndication(std::function<void(const V2xFrame&)> callback)
{
    m_NetTransmit = callback;
}

void Relay::SetNetDccIndication(std::function<void(const ChannelBusyRatio&)> callback)
{
    m_NetDccTransmit = callback;
}
