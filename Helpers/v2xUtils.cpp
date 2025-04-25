/*
 * v2xUtils.cpp
 *
 *  Created on: Sep 3, 2024
 *      Author: karel
 */

#include <cstdio>
#include <cstdint>
#include "v2xUtils.hpp"


void hexDump(const std::vector<uint8_t>& data)
{
    size_t size = data.size();
    printf("\n************************************************\n");
    printf("Data length %zu: \n", size);
    for (uint16_t i = 0; i < size; i++)
    {
        printf("%02x ", data[i]);
        if (i > 0 && (i + 1) % 16 == 0)
        printf("\n");
    }
    printf("\n************************************************\n\n");
}
