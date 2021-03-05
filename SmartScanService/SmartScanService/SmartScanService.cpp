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
	NewScan(scanId, sensorIds, sensorIds[0]);
}

void SmartScan::SmartScanService::NewScan(const std::vector<int> sensorIds, const int refSensorId, const double sampleRate)
{
	NewScan(FindNewScanId(), sensorIds, refSensorId, sampleRate);
}

void SmartScan::SmartScanService::NewScan(int scanId, const std::vector<int> sensorIds, const int refSensorId, const double sampleRate)
{
	//check if id is unique:
	if (IdExists(scanId))
	{
		throw ex_smartScan("Scan object ID must be unique", __func__, __FILE__);
	}
	//create new scan obj
	this->scans.emplace_back(std::make_shared<Scan>(scanId, tSCtrl, sampleRate, sensorIds, refSensorId));
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
		//set the resolution:
		scans.back()->SetFilteringPrecision(mFilteringPrecision);
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

void SmartScan::SmartScanService::CalibrateSingleRefPoint()
{
	if (scans.size() == 0)
	{
		throw ex_smartScan("No existing scan object found", __func__, __FILE__);
	}

	//only use thumb and index finger:
	std::vector<int> sensorsUsed = { mThumbSensorId,mIndexSensorId };

	scans.back()->SetUsedSensors(sensorsUsed);

	scans.back()->Run(true);

	ReferencePoint newRef;

	//wait for values:
	while (scans.back()->mInBuff.size() < 2 || scans.back()->mRefBuff.size() < 1)
	{
	}


	std::vector<Point3>::const_iterator firstFingerIterator = scans.back()->mInBuff.cend() - sensorsUsed.size();
	//add the referenceSensorPos:
	newRef.refSensorPos = scans.back()->mRefBuff.back();

	newRef.pos.x = ((firstFingerIterator[0].x + firstFingerIterator[1].x) / 2) - newRef.refSensorPos.x;
	newRef.pos.y = ((firstFingerIterator[0].y + firstFingerIterator[1].y) / 2) - newRef.refSensorPos.y;
	newRef.pos.z = ((firstFingerIterator[0].z + firstFingerIterator[1].z) / 2) - newRef.refSensorPos.z;

	scans.back()->AddReference(newRef);

	scans.back()->Stop(true);
	//reset used sensors:
	scans.back()->SetUsedSensors();
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

void SmartScan::SmartScanService::SetFilteringPrecision(const double precision)
{
	mFilteringPrecision = precision;
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
	std::cin.get();


	if (!refCount)
	{
		throw ex_smartScan("No reference point.", __func__, __FILE__);
	}


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
		//store the latest values:
		ReferencePoint newRef;

		//wait for values:
		while (scans.back()->mInBuff.size() < 2 || scans.back()->mRefBuff.size() < 1)
		{
		}

		std::cout << "[CALIBRATION] " << "Position your fingers around the reference point and press any key to capture it" << std::endl;
		std::cin.get();

		std::vector<Point3>::const_iterator firstFingerIterator = scans.back()->mInBuff.cend() - sensorsUsed.size();
		//add the referenceSensorPos:
		newRef.refSensorPos = scans.back()->mRefBuff.back();

		newRef.pos.x = ((firstFingerIterator[0].x + firstFingerIterator[1].x) / 2) - newRef.refSensorPos.x;
		newRef.pos.y = ((firstFingerIterator[0].y + firstFingerIterator[1].y) / 2) - newRef.refSensorPos.y;
		newRef.pos.z = ((firstFingerIterator[0].z + firstFingerIterator[1].z) / 2) - newRef.refSensorPos.z;

		// Reorientate the reference points to the 0-0-0 angles where all points are oriented to
		const double azimuth = newRef.refSensorPos.r.z;
		const double elevation = newRef.refSensorPos.r.y;
		const double roll = newRef.refSensorPos.r.x;
		// Declare new variables for new points
		double x_new = 0;
		double y_new = 0;
		double z_new = 0;
		const double pi = 3.14159265;

		// Use the azimuth to calculate the rotation around the z-axis
		const double azimuth_distance = sqrt(pow(newRef.pos.x, 2) + pow(newRef.pos.y, 2));
		const double a = (atan2(newRef.pos.y, newRef.pos.x) * 180 / pi) - azimuth;
		x_new = azimuth_distance * cos(a * pi / 180);
		y_new = azimuth_distance * sin(a * pi / 180);

		// Use the elevation to calculate the rotation around the y-axis
		const double elevation_distance = sqrt(pow(x_new, 2) + pow(newRef.pos.z, 2));
		const double b = (atan2(newRef.pos.z, x_new) * 180 / pi) + elevation;
		x_new = elevation_distance * cos(b * pi / 180);
		z_new = elevation_distance * sin(b * pi / 180);

		// Use the roll difference to calculate the rotation around the x-axis
		const double roll_distance = sqrt(pow(y_new, 2) + pow(z_new, 2));
		const double c = (atan2(z_new, y_new) * 180 / pi) - roll;
		y_new = roll_distance * cos(c * pi / 180);
		z_new = roll_distance * sin(c * pi / 180);

		newRef.pos.x = x_new;
		newRef.pos.y = y_new;
		newRef.pos.z = z_new;

		newRef.index = i;


		scans.back()->AddReference(newRef);
		std::cout << "[CALIBRATION] " << "Reference point at (" << newRef.pos.x << "," << newRef.pos.y << "," << newRef.pos.z << ") with index " << newRef.index << " set" << std::endl;
	}



	std::cout << "[CALIBRATION] " << "Done setting reference points" << std::endl;
	scans.back()->Stop(true);
	//reset used sensors:
	scans.back()->SetUsedSensors();
}
void SmartScanService::GetSensorInformation()
{
    if (!mUseMockData) {
        tSCtrl->PrintSensorInfo();
    }
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