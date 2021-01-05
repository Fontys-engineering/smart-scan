#include "SmartScanService.h"
#include "Exceptions.h"
#include <chrono>

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

	//calibrate reference points:
	CalibrateReferencePoints();

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

void SmartScanService::CalibrateReferencePoints()
{
	int refCount;
	double refSetTime = 5000;	//time in milliseconds after which the point is considered a reference.
	double tError = 20;			//tolerated translation error in mm
	double rError = 20;			//tolerated rotation error in mm

	//find how many calibration points are desired:
	std::cout << "Enter the number of desired reference points: ";
	std::cin >> refCount;

	//reset the Scan's reference points if some already exist:
	if (scans.back().GetReferences().size() > 0)
	{
		scans.back().ResetReferences();
	}
	//do this for the given number of ref points:
	for (int i = 0; i < refCount; i++)
	{
		bool refSet = false;
		//start reading sensor data:
		scans.back().Run();

		auto startTime = std::chrono::steady_clock::now();
		Point3 prevPoint;
		//wait for a 5 second stable reading (within a margin of error)
		while (!refSet)
		{
			Point3 currentPoint;
			if (scans.back().mInBuff.size())
			{
				currentPoint = scans.back().mInBuff.back();

				if (abs(currentPoint.x - prevPoint.x) > tError ||
					abs(currentPoint.y - prevPoint.y) > tError ||
					abs(currentPoint.z - prevPoint.z) > tError ||
					abs(currentPoint.r.x - prevPoint.r.x) > rError ||
					abs(currentPoint.r.x - prevPoint.r.x) > rError ||
					abs(currentPoint.r.x - prevPoint.r.x) > rError)
				{
					startTime = std::chrono::steady_clock::now();
				}
			}
			auto endTime = std::chrono::steady_clock::now();
			//check if 5 seconds have passed:
			if (std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() > refSetTime)
			{
				//ref found
				refSet = true;;
			}

			prevPoint = currentPoint;
		}
		//if stable point, add it to the scan:
		ReferencePoint newRef;
		newRef.index = i;
		newRef.pos = prevPoint;
		scans.back().AddReference(newRef);
		std::cout << "Reference point with index " << i << " set" << std::endl;
		if (i < refCount)
		{
			std::cout << "Press any key to move to the next one" << std::endl;
		}
	}

	std::cout << "Done setting reference points" << std::endl;
	scans.back().Stop();
}