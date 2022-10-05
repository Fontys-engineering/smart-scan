// This is the SmartScan trigger class.
// It contains the functions needed to detect button presses and incrementing the buttonstate.

#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "Point3.h"

namespace SmartScan
{
	class Trigger {
	public:
		// Returns the current button state.
		button_state GetButtonState();

		// Resets the buttonstate to INVALID.
		void ClearMyButton();

		// Update current button state with the current button value from the TrakStar device. 
		// Arguments:
		// -buttonBit : button value of the latest trakStar sample.
		void UpdateButtonState(unsigned short buttonBit);
	private:
		const double buttonDelayTime = 2.0;										// Button buffer change time.

		button_state buttonState = button_state::INVALID;						// Button member.
		int buttonBuffer = 0;													// Button state buffer.

		std::chrono::time_point<std::chrono::steady_clock> lastRiseTime;		// Time point last rising time.
	};
}