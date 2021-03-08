#pragma once

//system libraries:
#include <iostream>
#include <vector>

//custom libraries:
#include "SmartScanService.h"
#include "Point3.h"

const bool mockMode = false;

const std::vector<int> usedSensors = { 0,1 };
const unsigned int refSensorId = 2;

const double sampleRate = 80;
const double filteringPrecision = 4;

//create a new SmartScanService object with mock data:
SmartScan::SmartScanService s3(mockMode);

void Usage();

void RawPrintCallback(std::vector<SmartScan::Point3>& data);