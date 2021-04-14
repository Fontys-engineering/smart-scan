#include "Trigger.h"
#include <iostream>
using namespace SmartScan;

button_state Trigger::GetButtonState()
{
	// Return current button state
	return this->buttonState;
}

void Trigger::UpdateButtonState(unsigned short buttonBit)
{
	static unsigned short lastButtonBit = 0;
	static int risingEdge = 0;

	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - lastRiseTime;

	// Convert the enum to an integer
	int stateNum = static_cast<int>(buttonState);

	// Keep track of current time
	// After buttonDelayTime it change the current button state
	if (elapsedTime.count() >= buttonDelayTime && risingEdge == 1)
	{
		buttonBuffer = (buttonBuffer + stateNum) % 4;
		buttonState = static_cast<button_state>(buttonBuffer);
		//std::cout << "The current state: " << buttonBuffer << std::endl;
		risingEdge = 0;
		buttonBuffer = 0;
	}
	// Detect rising edge and store its time
	if (buttonBit != lastButtonBit && buttonBit == 1)
	{
		lastRiseTime = currentTime;
		buttonBuffer = buttonBuffer + 1;
		risingEdge = 1;
	}
	lastButtonBit = buttonBit;
}