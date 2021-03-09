#pragma once

#include <iostream>
#include <vector>

#include "SmartScanService.h"
#include "Point3.h"

const bool mockMode = true;

const std::vector<int> usedSensors = {1879, 1877, 1876};

const double sampleRate = 50;
const double filteringPrecision = 4;

// Create a new SmartScanService object with mock data:
SmartScan::SmartScanService s3(mockMode);

/// <summary>
/// Display the help screen
/// </summary>
void Usage();

void RawPrintCallback(std::vector<SmartScan::Point3>& data);