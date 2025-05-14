/*
 * linkLayer.cpp
 *
 *  Created on: Nov 6, 2024
 *      Author: karel
 */

#include <arpa/inet.h>
#include <ifaddrs.h>        /* To get interface-specific addresses */
#include "network.hpp"


/**
 * Get IP address string from an interface name. getifaddrs()-based approach.
 * @param  interfaceName String with the name of the interface
 * @retval String with the IP address or empty string in case of an error
 */
static std::string getIpAddressGetifaddrs(const std::string& interfaceName = "eth0");


std::string getIpAddressGetifaddrs(const std::string& interfaceName)
{
    std::string ret = "";
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) != -1)  // Use getifaddrs to retrieve the list of network interfaces
    {
        for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            // Ensure the interface is valid and has an address
            if (ifa->ifa_addr != NULL)
            {
                // Check if the interface matches the specified name and is AF_INET (IPv4)
                if ((interfaceName == ifa->ifa_name) && (ifa->ifa_addr->sa_family == AF_INET))
                {
                    char addr_buf[INET_ADDRSTRLEN];
                    void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;

                    // Convert binary address to string
                    if (inet_ntop(AF_INET, addr, addr_buf, INET_ADDRSTRLEN) != NULL)
                    {
                        ret = addr_buf;
                    }
                    break; // Exit once the desired interface is found
                }
            }
        }
        freeifaddrs(ifaddr);
        // Note: IP address may not have been found
    }
    return ret;
}

std::string parseEndpoint(const NetConfig& netConfig)
{
    std::string ipParam = netConfig.IpAddress;
    std::string ipFromName = "";
    if ("" != netConfig.EthName)
    {
        ipFromName = getIpAddressGetifaddrs(netConfig.EthName);
        if ("" != ipFromName)
        {
            ipParam = ipFromName;
        }
    }
    return ipParam + ":" + netConfig.Port;
}
