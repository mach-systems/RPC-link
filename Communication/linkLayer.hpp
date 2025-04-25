/*
 * linkLayer.hpp
 *
 *  Created on: Nov 6, 2024
 *      Author: karel
 */

#ifndef LINKLAYER_HPP_
#define LINKLAYER_HPP_

#include <string>

#include "relay.hpp"


/**
 * Default TCP/UDP port
 */
const std::string PortDefault("8947");

/**
 * Link layer connection configuration
 */
struct RelayConfig
{
    std::string IpAddress;      /* IP address for reception */
    std::string Port;           /* Port to use both for reception */
    std::string EthName;        /* Optional: Ethernet interface name for automatic IP address */
};


/**
 * Determine endpoint (IP + port) from the given parameters. If eth is specified, it is used as an
 * interface name for determining the IP address. If it is not, ip+port is just combined together.
 * @param  relayConfig Configuration structure
 * @retval std::string with pair ip:port (not checked)
 */
std::string parseEndpoint(const RelayConfig& relayConfig);



#endif /* LINKLAYER_HPP_ */
