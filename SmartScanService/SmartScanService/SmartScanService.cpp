#include "SmartScanService.h"
#include "Exceptions.h"

using namespace SmartScan;

SmartScanService::SmartScanService(bool useMockData) : mUseMockData {useMockData}
{
}

SmartScanService::~SmartScanService()
{
	scans.clear();
}

void SmartScanService::Init()
{
	tSCtrl = new TrakStarController(mUseMockData);
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

void SmartScanService::DumpScan() const
{
	scans.back().DumpData();
}

void SmartScanService::ExportCSV(const std::string filename)
{
	if (scans.empty())
	{
		throw "No measurement available for export";
	}
	csvExport.ExportPoint3(scans.back().mOutBuff, filename);
}