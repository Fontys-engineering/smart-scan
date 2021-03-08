#pragma once

#include <iostream>
#include <vector>

#include "SmartScanService.h"
#include "Point3.h"

const bool mockMode = true;

const std::vector<int> usedSensors = {0,1};
const unsigned int refSensorId = 2;

const double sampleRate = 80;
const double filteringPrecision = 4;

// Create a new SmartScanService object with mock data:
SmartScan::SmartScanService s3(mockMode);

/// <summary>
/// Display the help screen
/// </summary>
void Usage();

void RawPrintCallback(std::vector<SmartScan::Point3>& data);