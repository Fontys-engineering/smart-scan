#pragma once

#include <vector>
#include <iostream>
#include <chrono>

#include "Point3.h"

namespace SmartScan
{
	class Trigger
	{
	public:
		/// <summary>
		/// Get current button state.
		/// </summary>
		button_state GetButtonState();

		/// <summary>
		/// Update current button state. 
		/// </summary>
		/// <param name="buttonBit"> - button value (0 or 1).</param>
		void UpdateButtonState(unsigned short buttonBit);

	private:
		const double buttonDelayTime = 2.0;										// Button buffer change time
		button_state buttonState = button_state::INVALID;						// Button member
		int buttonBuffer = 0;													// Button state buffer
		std::chrono::time_point<std::chrono::steady_clock> lastRiseTime;		// Time point last rising time
	};
}