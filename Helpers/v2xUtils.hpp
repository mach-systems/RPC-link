/*
 * v2xUtils.hpp
 *
 *  Created on: Sep 3, 2024
 *      Author: karel
 */

#ifndef V2XUTILS_HPP_
#define V2XUTILS_HPP_

#include <vector>


/**
 * Nicely print a hex buffer.
 * @param  data Vector with the data
 * @retval None
 */
void hexDump(const std::vector<uint8_t>& data);



#endif /* V2XUTILS_HPP_ */
