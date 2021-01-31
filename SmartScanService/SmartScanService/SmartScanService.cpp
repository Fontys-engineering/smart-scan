#include "SmartScanService.h"
#include "Exceptions.h"
#include <chrono>
#include <iomanip>

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
void SmartScanService::NewScan(const std::vector<int> sensorIds)
{
	//find the first unused id,
	//create the scan with that id
	NewScan(FindNewScanId(), sensorIds);
}

void SmartScanService::NewScan(int scanId, const std::vector<int> sensorIds)
{
	//check if id is unique:
	if (IdExists(scanId))
	{
		throw ex_smartScan("Scan object ID must be unique", __func__, __FILE__);
	}
	//create new scan obj
	this->scans.emplace_back(std::make_shared<Scan>(scanId, tSCtrl));

	//use the specified sensors (if specified)
	if (sensorIds.size() > 0)
	{
		this->scans.back()->SetUsedSensors(sensorIds);
	}
}


void SmartScanService::DeleteScan()
{
	if (scans.size() > 0)
	{
		this->scans.erase(scans.end() - 1);
	}
	else {
		throw ex_smartScan("No scans left to delete", __func__, __FILE__);
	}
}

void SmartScanService::DeleteScan(int id)
{
	bool ok = false;
	for (int s = 0; s < scans.size(); s++)
	{
		if (scans[s]->mId == id) {
			this->scans.erase(scans.begin() + s);
			ok = true;
			break;
		}
	}
	if (!ok)
	{
		throw ex_smartScan("Scan id not found", __func__, __FILE__);
	}
}
void SmartScanService::StartScan(const std::vector<int> sensorIds)
{
	//create new scan obj if none exists or the existing one is already running:
	if (!scans.size() || scans.back()->isRunning())
	{
		this->scans.emplace_back(std::make_shared<Scan>(0, tSCtrl));
	}

	//use the specified sensors (if specified)
	if (sensorIds.size() > 0)
	{
		this->scans.back()->SetUsedSensors(sensorIds);
	}

	//check if reference points have been set;
	if (!scans.back()->GetReferences().size())
	{
		throw ex_smartScan("cannot start scan without reference points set.", __func__, __FILE__);
	}

	//start the scan:
	try
	{
		//if UI callback is available, register it with this new Scan:
		if (mUICallback)
		{
			this->scans.back()->RegisterNewDataCallback(mUICallback);
		}
		scans.back()->Run();
	}
	catch (ex_scan e)
	{
		throw e;
	}
	catch (ex_trakStar e)
	{
		throw e;
	}
	catch (ex_smartScan e)
	{
		throw e;
	}
	catch (...)
	{
		throw "Cannot start scan";
	}
}

void SmartScanService::StartScan(int scanId, const std::vector<int> sensorIds)
{
	if (!IdExists(scanId))
	{
		//create new scan obj with that id:
		if (!scans.size() || scans.back()->isRunning())
		{
			this->scans.emplace_back(std::make_shared<Scan>(0, tSCtrl));
		}
	}
	//use the specified sensors (if specified)
	if (sensorIds.size() > 0)
	{
		this->scans.back()->SetUsedSensors(sensorIds);
	}

	//start the scan:
	try
	{
		//if UI callback is available, register it with this new Scan:
		if (mUICallback)
		{
			this->scans.back()->RegisterNewDataCallback(mUICallback);
		}
		scans.back()->Run(false);
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

void SmartScanService::SetReferencePoints(const std::vector<ReferencePoint> referencePoints)
{
	if (scans.size() < 0)
	{
		std::cout << "[SMART SCAN] " << "Cannot set reference points because no new scan has been created." << std::endl;
		return;
	}
	if (scans.back()->GetReferences().size() > 0)
	{
		//reset the reference points:
		scans.back()->ResetReferences();
	}
	for (auto rp : referencePoints)
	{
		scans.back()->AddReference(rp);
	}
}

void SmartScanService::StopScan()
{
	scans.back()->Stop();
}

void SmartScanService::DumpScan() const
{
	scans.back()->DumpData();
}

void SmartScanService::SetUsedSensors(const std::vector<int> sensorIds)
{
	scans.back()->SetUsedSensors(sensorIds);
}

const std::shared_ptr<Scan> SmartScanService::GetScan() const
{
	return scans.back();
}
const std::shared_ptr<Scan> SmartScanService::GetScan(int id) const
{
	for (int s = 0; s < scans.size(); s++)
	{
		if (scans[s]->mId == id) {
			return scans[s];
		}
	}
	throw ex_smartScan("Scan id not found", __func__, __FILE__);
}

const std::vector<std::shared_ptr<Scan>>& SmartScanService::GetScansList() const
{
	return scans;
}

void SmartScanService::ExportCSV(const std::string filename, const bool raw)
{
	if (scans.empty())
	{
		throw "No measurement available for export";
	}
	if (raw)
	{
		csvExport.ExportPoint3(scans.back()->mInBuff, filename);
	}
	else
	{
		csvExport.ExportPoint3(scans.back()->mOutBuff, filename);
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
		csvExport.ExportPC(scans.back()->mInBuff, filename);
	}
	else
	{
		csvExport.ExportPC(scans.back()->mOutBuff, filename);
	}
}

void SmartScanService::RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mUICallback = callback;
	//register this callback with all the existing Scans:
	for (auto& scan : scans)
	{
		scan->RegisterNewDataCallback(mUICallback);
	}
}

void SmartScanService::RegisterRawDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mRawCallback = callback;
	//register this callback with all the existing Scans:
	for (auto& scan : scans)
	{
		scan->RegisterRawDataCallback(mRawCallback);
	}
}

void SmartScanService::CalibrateReferencePoints()
{
	int refCount;

	//find how many calibration points are desired:
	std::cout << "[CALIBRATION] " << "Before starting the scan, reference points must be calibrated. Use the thumb and index finger to point out the knee, ankle and foot ecnter \n";
	std::cout << "[CALIBRATION] " << "Enter the number of desired reference points  (by default 3, as mentioned above): ";
	std::cin >> refCount;

	if (!refCount)
	{
		throw ex_smartScan("No reference point.", __func__, __FILE__);
	}

	std::cout << "[CALIBRATION] " << "Position your fingers on the first reference point and press any key to start." << std::endl;
	std::cin.get();

	//reset the Scan's reference points if some already exist:
	if (scans.back()->GetReferences().size() > 0)
	{
		scans.back()->ResetReferences();
	}
	//only use thumb and index finger:
	std::vector<int> sensorsUsed = { mThumbSensorId,mIndexSensorId };
	scans.back()->SetUsedSensors(sensorsUsed);

	//start reading sensor data:
	std::cout << "[CALIBRATION] " << "A temporary scan will run for the duration of the calibration. The data will be deleted afterwards." << std::endl;
	//acquisition only
	scans.back()->Run(true);
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

		unsigned long recordId = 0;
		while (!refSet)
		{
			std::cout << "\r";
			for (unsigned int fingerIndex = 0; fingerIndex < prevFrame.size(); fingerIndex++)
			{
				if (scans.back()->mInBuff.size() > recordId)
				{
					currentFrame[fingerIndex] = scans.back()->mInBuff[recordId];
					++recordId;

					if (abs(currentFrame[fingerIndex].x - prevFrame[fingerIndex].x) > tError ||
						abs(currentFrame[fingerIndex].y - prevFrame[fingerIndex].y) > tError ||
						abs(currentFrame[fingerIndex].z - prevFrame[fingerIndex].z) > tError)
					{
						startTime = std::chrono::steady_clock::now();
					}

					std::cout << std::setprecision(5) << currentFrame[fingerIndex].x << "\t" << currentFrame[fingerIndex].y << "\t" << currentFrame[fingerIndex].z << "\t";
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

		//add the referenceSensorPos:
		newRef.refSensorPos = scans.back()->mRefBuff.back();

		scans.back()->AddReference(newRef);
		std::cout << "[CALIBRATION] " << "Reference point at (" << newRef.pos.x << "," << newRef.pos.y << "," << newRef.pos.z << ") with index " << newRef.index << " set" << std::endl;
		if (i < refCount)
		{
			prevFrame.clear();
			currentFrame.clear();

			if (i < refCount-1)
			{
				std::cout << "[CALIBRATION] " << "Press any key to move to the next one" << std::endl;
				std::cin.get();
			}
		}
	}

	std::cout << "[CALIBRATION] " << "Done setting reference points" << std::endl;
	scans.back()->Stop(true);
	//reset used sensors:
	scans.back()->SetUsedSensors();
}

const int SmartScanService::FindNewScanId() const
{
	int newId = 0;
	for (int scn = 0; scn < scans.size(); scn++)
	{
		if (newId == scans[scn]->mId)
		{
			++newId;
		}
	}

	return newId;
}

const bool SmartScanService::IdExists(const int scanId) const
{
	for (int scn = 0; scn < scans.size(); scn++)
	{
		if (scanId == scans[scn]->mId)
		{
			return true;
		}
	}
	return false;
}