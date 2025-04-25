/*
 * led.cpp
 *
 *  Created on: Jan 2, 2024
 *      Author: karel
 */

#include <cstdint>
#include "led.hpp"


namespace Led
{

LedControl::LedControl(kj::Timer& timer)
                      : m_Timer(timer)
{
    ledInit();
}

LedControl::~LedControl(void)
{
    ledDeinit();
}

kj::Promise<void> LedControl::ScheduleTimer(void)
{
    // Control callback period
    const uint8_t LED_CONTROL_PERIOD = 100;

    auto promise = m_Timer.afterDelay(LED_CONTROL_PERIOD * kj::MILLISECONDS)
        .then([this]() -> kj::Promise<void> {
    	    // Periodical timer callback e.g. for LED control

            // Reschedule the timer
            return ScheduleTimer();
        });
    return promise;
}

void LedControl::ledInit(void)
{
	// LED initialization
}

void LedControl::ledDeinit(void)
{
	// LED deinitialization
}


} // namespace Led
