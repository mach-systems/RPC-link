/*
 * autotalksLink.h
 *
 *  Created on: Dec 22, 2021
 *      Author: karel
 */

#ifndef AUTOTALKSLINK_HPP_
#define AUTOTALKSLINK_HPP_

#include <functional>
#include <memory>   /* std::unique_ptr */

#include "defs.hpp"
#include "led.hpp"


class AutotalksLink
{
    public:
        AutotalksLink(bool, Led::LedControl&);

        /*
         * Request sending of a packet.
         */
        void request(V2xFrame& frame) const;

        /*
         * Callback when data is received.
         */
        void data_received(const V2xFrame& frame);

        /**
         * Called by lower layer when CBR was measured.
         * @param  cbr Measured CBR for DCC
         * @retval None
         */
        void DccMeasuredCallback(double cbr);

        /**
         * Set relay indication callback (when V2X packet was received).
         * @param  callback Callback function to call when sending to net is requested
         * @retval None
         */
        void SetNetIndication(std::function<void(const V2xFrame&)> callback);

        /**
         * Set CBR measurement callback.
         * @param  callback Callback function to call when CBR measurement was done
         * @retval None
         */
        void SetNetDcc(std::function<void(double)> callback);

    protected:
        /*
         * Flag if print the whole physical layer buffer
         */
        bool m_Print;

        Led::LedControl& m_Led;

        /**
         * Inform about DCC measurement.
         * @param  DCC measurement
         * @retval None
         */
        std::function<void(double)> m_SendDcc;

        /**
         * Callback that some V2X data was received.
         * @param  Received data
         * @param  Ethernet header
         * @retval None
         */
        std::function<void(const V2xFrame&)> m_PacketRx;
};



#endif /* AUTOTALKSLINK_HPP_ */
