#include "SmartScanService.h"
#include "Exceptions.h"

using namespace SmartScan;

SmartScanService::SmartScanService()
{

}

SmartScanService::~SmartScanService()
{
	scans.clear();
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
	try
	{
		scans.back().Run();
	}
	catch(ex_scan e)
	{
		throw e;
	}
	catch (ex_trakStar e)
	{
		throw e;
	}
	catch (std::exception e)
	{
		throw e;
	}
	catch (...)
	{
		throw "Cannot start scan";
	}
}

void SmartScanService::StopScan()
{
	scans.back().Stop();
}

void SmartScanService::DumpScan()
{
	scans.back().DumpData();
}