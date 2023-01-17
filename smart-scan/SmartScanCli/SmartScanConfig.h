// Contains all the options needed for controlling the SmartScanService.

#pragma once

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "SmartScanService.h"

// Glove setup
int rThumbSerial = 55435;						// Only used for setting reference points and ZOffsets calibration.
int rIndexSerial = 55429;						// Only used for setting reference points and ZOffsets calibration.
int rMiddleSerial = 55430;						// Only used for setting reference points and ZOffsets calibration.

// Data-acquisition configuration
short int transmitterID = 0;                    // Port of the transmitter, is usually 0 with one trakStar device.
double measurementRate = 255;                   // Between 20.0 and 255.0
double powerLineFrequency = 50.0;               // Either 50.0 or 60.0
double maximumRange = 36.0;                     // Either 36.0 (914,4 mm), 72.0 and 144.0.
int refSensorSerial = 55432;					// Serial number of the reference sensor, set as -1 when no reference sensor is used.
double frameRotations[3] = {0, 0, 0};			// Set the rotation of the measurement frame, azimuth, elevation and roll. (0, 0, 0) is default. 
SmartScan::DataAcqConfig acquisitionConfig = {transmitterID, measurementRate, powerLineFrequency, maximumRange, refSensorSerial, frameRotations};
