/*
 * autotalksLink.cpp
 *
 * For RSSI measurements see branch RiverdiDemo2023.
 *
 *  Created on: Dec 22, 2021
 *      Author: karel
 */

#include <cstdio>
#include "autotalksLink.hpp"
#include "autotalks.hpp"
#include "v2xUtils.hpp"


#define TX_BUFFER_SIZE      2048


AutotalksLink::AutotalksLink(bool print, Led::LedControl& led)
                             : m_Print(print),
                               m_Led(led)
{
    // Initialize reception
    initRxThread(this);
    // Initialize CBR measurement
    initCbrThread(this);
}

void AutotalksLink::request(V2xFrame& frame) const
{
    autotalksTransmit(frame);
    // Here LEDs could be toggled
}

void AutotalksLink::DccMeasuredCallback(double cbr)
{
    if (m_SendDcc)
    {
        m_SendDcc(cbr);
    }
}

void AutotalksLink::data_received(const V2xFrame& frame)
{
    if (m_Print)
    {
        (void) printf("V2X RX Data (networking layer):");
        hexDump(frame.Data);
    }
    if (m_PacketRx)
    {
        m_PacketRx(frame);
    }
}

void AutotalksLink::SetNetIndication(std::function<void(const V2xFrame&)> callback)
{
    m_PacketRx = callback;
}

void AutotalksLink::SetNetDcc(std::function<void(double)> callback)
{
    m_SendDcc = callback;
}
