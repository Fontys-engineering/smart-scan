#pragma once

//system libraries:
#include <iostream>
#include <vector>

//custom libraries:
#include "SmartScanService.h"
#include "Point3.h"

const bool mockMode = true;
const std::vector<int> usedSensors = { 0,1,2 };

void Usage();

void TestUICallback(std::vector<SmartScan::Point3>& data);