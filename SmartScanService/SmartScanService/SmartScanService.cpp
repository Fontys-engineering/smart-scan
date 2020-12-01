#include "SmartScanService.h"


using namespace SmartScan;

SmartScanService::SmartScanService()
{

}

void SmartScanService::Init()
{
	tSCtrl = new TrakStarController();
	tSCtrl->Init();
	tSCtrl->Config();
	tSCtrl->AttachSensor();
}

void SmartScanService::StartScan()
{
	//create new scan obj
	this->scans.emplace_back(Scan(0, tSCtrl));
	//create a new thread for this scan:
	scans.back().Run();
}

void SmartScanService::StopScan()
{
	scans.back().Stop();
}