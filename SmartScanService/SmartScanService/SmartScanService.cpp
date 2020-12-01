#include "SmartScanService.h"

using namespace SmartScan;

void SmartScanService::StartScan()
{
	//create new scan obj
	this->scans.emplace_back(new Scan(0));
	//create a new thread for this scan:
	
}