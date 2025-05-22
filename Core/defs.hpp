/*
 * defs.hpp
 *
 *  Created on: Feb 16, 2022
 *      Author: karel
 */

#ifndef DEFS_HPP_
#define DEFS_HPP_


#include <cstdint>
#include <vector>


/**
 * RX print toggle; TODO make this as parameter from command line
 */
//#define RX_PRINT

#define MAC_ADDR_LEN            6U

/**
 * V2X frame structure for RPC communication
 */
struct V2xFrame
{
    std::vector<uint8_t> SrcAddress;    /*< Source address */
    std::vector<uint8_t> DstAddress;    /*< Destination address */
    std::vector<uint8_t> Data;          /*< Frame data */
    /**
     * Parameters for WLAN devices in OCB mode (IEEE 802.11 p and bd)
     */
    struct WlanParameters_t
    {
        uint8_t Priority;   /*< 802.1 user priority (0-7) */
        int16_t Power;      /*< dBm scaled by 8 */
        uint16_t Datarate;  /*< Mbps scaled by 2 (500kbps steps) */
    };
    /**
     * Parameters for C-V2X devices (LTE-V2X and 5G-V2X)
     */
    struct Cv2xParameters_t
    {
        uint8_t Priority;   /*< PPPP (0-7) */
        int16_t Power;      /*< dBm scaled by 8 */
    };
    WlanParameters_t WlanParameters;
    Cv2xParameters_t Cv2xParameters;
    uint64_t Timestamp;		/*< Timestamp (used for V2X RX) */
};

/**
 * Channel busy ration for RPC communication
 */
struct ChannelBusyRatio
{
    uint16_t Busy;      /*< Number of samples sensed as busy */
    uint16_t Samples;   /*< Total number of samples in measurement interval */
};



#endif /* DEFS_HPP_ */
