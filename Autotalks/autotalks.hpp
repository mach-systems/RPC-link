/*
 * autotalks.hpp
 *
 *  Created on: Dec 22, 2021
 *      Author: karel
 */

#ifndef AUTOTALKS_HPP_
#define AUTOTALKS_HPP_

#include "autotalksLink.hpp"
#include "defs.hpp"


/*
 * Initialize the device.
 */
int v2x_device_init(void);

/**
 * Initialize end - set boolean variables for the thread to end their execution.
 * @param  None
 * @retval None
 */
void SetThreadEnd(void);

/*
 * Deinitialize the device.
 */
int autotalks_device_deinit(void);

/*
 * Create autotalks header and send the packet.
 */
void autotalksTransmit(const V2xFrame&);

/*
 * Initialize reception thread.
 */
void initRxThread(AutotalksLink* pLinkLayer);

/*
 * Initialize CBR measurement thread.
 */
void initCbrThread(AutotalksLink* pLinkLayer);

/**
 * Set MAC or L2ID address.
 * @param  address Take the address from here
 * @retval None
 */
bool SetAddress(const std::vector<uint8_t>& address);



#endif /* AUTOTALKS_HPP_ */
