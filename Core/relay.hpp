/*
 * relay.hpp
 *
 *  Created on: Mar 4, 2025
 *      Author: karel
 */

#ifndef RELAY_HPP_
#define RELAY_HPP_

#include "autotalksLink.hpp"
#include "defs.hpp"


class Relay
{
    public:
        /**
         * Constructor.
         * @param  None
         * @retval None
         */
        Relay(AutotalksLink& V2XLink, bool physPrint);

        /**
         *
         */
        void DccMeasuredCallback(double cbr);

        /**
         * Request to send V2X packet.
         * @param  packet Packet to transmit
         * @retval None
         */
        void PacketSend(V2xFrame& frame) const;

        /**
         * Callback when V2X packet was received.
         * @param  frame Received frame
         * @retval None
         */
        void PacketReceived(const V2xFrame& frame);

        /**
         * Set network indication callback.
         * @param  callback Callback function to call when sending to net is requested
         * @retval None
         */
        void SetNetIndication(std::function<void(const V2xFrame&)> callback);

        /**
         * Set network indication callback for DCC measurement.
         * @param  callback Callback function to call when sending of CBR to net is required
         * @retval None
         */
        void SetNetDccIndication(std::function<void(const ChannelBusyRatio&)> callback);

    private:
        /**
         * Transmit a message to the network
         */
        std::function<void(const V2xFrame&)> m_NetTransmit;

        /**
         * Transmit CBR a message to the network
         */
        std::function<void(const ChannelBusyRatio&)> m_NetDccTransmit;

        /**
         * Reference to the Autotalks link layer handler
         */
        AutotalksLink& m_V2XLink;

        /**
         * Print TX data on physical layer or not
         */
        bool m_PhysPrint;
};



#endif /* RELAY_HPP_ */
