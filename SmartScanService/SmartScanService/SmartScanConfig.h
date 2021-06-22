#pragma once

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "SmartScanService.h"
#include "Point3.h"

// Set application to use mock mode
const bool mockMode = true; 

// Glove setup
int rThumbSerial = 1;
int rIndexSerial = 2;
//int rMiddleSerial = 1;
//int rRingSerial = 1;
//int rPinkySerial = 1;

// Data-acquisition configuration
short int transmitterID = 0;                    // Port of the transmitter, is usually 0 with one trakStar device.
double measurementRate = 50;                    // Between 20.0 and 255.0
double powerLineFrequency = 50.0;               // Either 50.0 or 60.0
double maximumRange = 36.0;                     // Either 36.0 (914,4 mm), 72.0 and 144.0.
int refSensorSerial = -1;					    // Serial number of the reference sensor, set as -1 when no reference sensor is used.
bool useMatrix = true;							// Use rotation matrixes to correct for the reference sensor instead of euler angles.
SmartScan::DataAcqConfig acquisitionConfig = {transmitterID, measurementRate, powerLineFrequency, maximumRange, refSensorSerial, useMatrix};

// Scan configuration
std::vector<int> usedSerials = {rThumbSerial, rIndexSerial};
std::vector<SmartScan::Point3> refPoints;
unsigned filteringPrecision;