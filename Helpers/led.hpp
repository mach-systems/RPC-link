/*
 * led.hpp
 *
 *  Created on: Jan 2, 2024
 *      Author: karel
 *
 *  LED control module - V2X and GPS LEDs
 *
 */

#ifndef LED_HPP_
#define LED_HPP_


#include <kj/async-io.h>


namespace Led
{


/**
 * LED control class
 */
class LedControl
{
    public:
        /**
         * Constructor
         * @param io Reference to kj::Timer used for timer
         * @retval None
         */
        LedControl(kj::Timer& timer);

        /**
         * Destructor - turn all the LEDs off
         * @param None
         * @retval None
         */
        ~LedControl(void);

        /**
         * Clear all the LEDs
         * @param None
         * @retval None
         */
        void ledDeinit(void);

        /**
         * Schedule new timer callback with period LED_CONTROL_PERIOD milliseconds.
         * @param waitScope KJ wait scope
         * @retval Promise to wait on
         */
        kj::Promise<void> ScheduleTimer(void);

    protected:
        /**
         * Initialize GNSS, V2X1 and V2X2 LEDs
         * @param None
         * @retval None
         */
        void ledInit(void);

        /**
         * Periodical timer for control callback
         */
        kj::Timer& m_Timer;
};


} // namespace Led

#endif /* LED_HPP_ */
