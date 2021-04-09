#pragma once

#include <iostream>
#include <vector>

#include "SmartScanService.h"
#include "Point3.h"

const bool mockMode = false;

SmartScan::ScanConfig config = {
    true,                   // acquisitionOnly
    false,                  // useReferenceSensor
    0,                      // referenceSensorId
    {1879, 1877},           // usedSensorIds
    50,                     // sampleRate
    4                       // filteringPrecision
};

// Create a new SmartScanService object with mock data:
SmartScan::SmartScanService s3(mockMode);

/// <summary>
/// Display the help screen
/// </summary>
void Usage();

void RawPrintCallback(std::vector<SmartScan::Point3>& data);