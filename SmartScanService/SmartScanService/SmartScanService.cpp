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
		//if UI callback is available, register it with this new Scan:
		if (mUICallback)
		{
			this->scans.back().RegisterNewDataCallback(mUICallback);
		}
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

void SmartScanService::ExportCSV(const std::string filename, const bool raw)
{
	if (scans.empty())
	{
		throw "No measurement available for export";
	}
	if (raw)
	{
		csvExport.ExportPoint3(scans.back().mInBuff, filename);
	}
	else
	{
		csvExport.ExportPoint3(scans.back().mOutBuff, filename);
	}
}

void SmartScanService::ExportPointCloud(const std::string filename, const bool raw)
{
	if (scans.empty())
	{
		throw "No measurement available for export";
	}
	if(raw)
	{
		csvExport.ExportPC(scans.back().mInBuff, filename);
	}
	else
	{
		csvExport.ExportPC(scans.back().mOutBuff, filename);
	}
}

void SmartScanService::RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mUICallback = callback;
	//register this callback with all the existing Scans:
	for (auto& scan : scans)
	{
		scan.RegisterNewDataCallback(mUICallback);
	}
}