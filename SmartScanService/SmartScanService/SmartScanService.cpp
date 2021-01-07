#include "SmartScanService.h"
#include "Exceptions.h"
#include <chrono>

using namespace SmartScan;

SmartScanService::SmartScanService(bool useMockData) : mUseMockData{ useMockData }
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
	if (this->scans.back().GetReferences().size() == 0)
	{
		CalibrateReferencePoints();
	}

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
	catch (ex_scan e)
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

const Scan& SmartScanService::GetScan() const
{
	return scans.back();
}
const Scan& SmartScanService::GetScan(int id) const
{
	for (int s = 0; s < scans.size(); s++)
	{
		if (scans[s].mId == id) {
			return scans[s];
		}
	}
	throw ex_smartScan("Scan id not found", __func__, __FILE__);
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
	if (raw)
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
	double tError = 10;			//tolerated translation error in mm
	double rError = 10;			//tolerated rotation error in mm

	//find how many calibration points are desired:
	std::cout << "[CALIBRATION] " << "Before starting the scan, reference points must be calibrated. Use the thumb and index finger to point out the knee, ankle and foot ecnter \n";
	std::cout << "[CALIBRATION] " << "Enter the number of desired reference points  (by default 3, as mentioned above): ";
	std::cin >> refCount;

	std::cout << "[CALIBRATION] " << "Position your fingers on the first reference point and press any key to start." << std::endl;
	std::cin.get();

	//reset the Scan's reference points if some already exist:
	if (scans.back().GetReferences().size() > 0)
	{
		scans.back().ResetReferences();
	}
	//only use thumb and index finger:
	std::vector<int> sensorsUsed = { mThumbSensorId,mIndexSensorId };
	scans.back().SetUsedSensors(sensorsUsed);

	//start reading sensor data:
	scans.back().Run();
	//do this for the given number of ref points:
	for (int i = 0; i < refCount; i++)
	{
		std::cout << "[CALIBRATION] " << "Calibration started..." << std::endl;
		bool refSet = false;

		auto startTime = std::chrono::steady_clock::now();
		//store the preious thumb and index finger position;
		std::vector<Point3> prevFrame(sensorsUsed.size());
		std::vector<Point3> currentFrame(sensorsUsed.size());
		//wait for a 5 second stable reading (within a margin of error)
		while (!refSet)
		{
			int buffSize = scans.back().mInBuff.size();
			if (buffSize >= sensorsUsed.size() && buffSize % sensorsUsed.size() == 0)
			{
				for (int fingerIndex = 0; fingerIndex < prevFrame.size(); fingerIndex++)
				{
					//read the data for the finger from the buffer:
					currentFrame[fingerIndex] = scans.back().mInBuff[(buffSize - fingerIndex - 1)];

					if (abs(currentFrame[fingerIndex].x - prevFrame[fingerIndex].x) > tError ||
						abs(currentFrame[fingerIndex].y - prevFrame[fingerIndex].y) > tError ||
						abs(currentFrame[fingerIndex].z - prevFrame[fingerIndex].z) > tError)
					{
						startTime = std::chrono::steady_clock::now();
					}
				}
			}
			auto endTime = std::chrono::steady_clock::now();
			//check if 5 seconds have passed:
			if (std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() > refSetTime)
			{
				//ref found
				refSet = true;;
			}

			prevFrame = currentFrame;
		}
		//if stable point, add it to the scan:
		ReferencePoint newRef;
		newRef.index = i;
		//find the midpoint:
		if (prevFrame.size() < 2)
		{
			throw ex_scan("calibration records not correct", __func__, __FILE__);
		}
		newRef.pos.x = ((prevFrame[0].x + prevFrame[1].x) / 2);
		newRef.pos.y = ((prevFrame[0].y + prevFrame[1].y) / 2);
		newRef.pos.z = ((prevFrame[0].z + prevFrame[1].z) / 2);

		scans.back().AddReference(newRef);
		std::cout << "[CALIBRATION] " << "Reference point at (" << newRef.pos.x << "," << newRef.pos.y << "," << newRef.pos.z << ") with index " << newRef.index << " set" << std::endl;
		if (i < refCount)
		{
			prevFrame.clear();
			currentFrame.clear();

			std::cout << "[CALIBRATION] " << "Press any key to move to the next one" << std::endl;
			std::cin.get();
		}
	}

	std::cout << "[CALIBRATION] " << "Done setting reference points" << std::endl;
	scans.back().Stop();
	//reset used sensors:
	scans.back().SetUsedSensors();
}
