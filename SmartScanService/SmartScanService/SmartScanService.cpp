#include <chrono>
#include <iomanip>

#include "Exceptions.h"
#include "SmartScanService.h"
#include "DataAcquisition.h"

using namespace SmartScan;

SmartScanService::SmartScanService(bool useMockData) : mUseMockData{ useMockData }, mDataAck(useMockData)
{

}

SmartScanService::~SmartScanService()
{
	scans.clear();
}

void SmartScanService::Init()
{
	mDataAck.Init();
}

void SmartScanService::Init(DataAcqConfig acquisitionConfig)
{
	mDataAck.Init(acquisitionConfig);
}

void SmartScanService::NewScan(ScanConfig config)
{
	if (180%config.filteringPrecision != 0) {
		throw ex_smartScan("180 is not a multiple of the filtering precision", __func__, __FILE__);
	}
	config.inBuff = mDataAck.getRawBuffer();
	this->scans.emplace_back(std::make_shared<Scan>(FindNewScanId(), config));
}

void SmartScanService::DeleteScan()
{
	this->scans.clear();
}

void SmartScanService::DeleteScan(int id)
{
	bool ok = false;
	for (int s = 0; s < scans.size(); s++) {
		if (scans[s]->mId == id) {
			this->scans.erase(scans.begin() + s);
			ok = true;
			break;
		}
	}
	if (!ok) {
		throw ex_smartScan("Scan id not found", __func__, __FILE__);
	}
}

void SmartScanService::StartScan()
{
	char arg[51];
	const char startString[] = "Cannot start scan ";
	const char endString[] = " due to reference points set.";
	char scan[3];

	// Start the scan:
	mDataAck.Start();

	for (int i = 0; i < scans.size(); i++) {
		// Check if reference points have been set;
		if (!this->scans.at(i)->NumRefPoints() && !this->scans.at(i)->NumUsedSensors()) {
			sprintf_s(scan, "%d", i);
			strcpy_s(arg, startString);
			strcat_s(arg, scan);
			strcat_s(arg, endString);
			throw ex_smartScan(arg, __func__, __FILE__);
		}

		scans.at(i)->Run();
	}
}

void SmartScanService::StartScan(int scanId)
{
	if (!IdExists(scanId)) {
        throw ex_smartScan("Scan id does not exist", __func__, __FILE__);
	}

	// Check if reference points and sensors have been set
	if (!this->scans.at(scanId)->NumRefPoints() && !this->scans.at(scanId)->NumUsedSensors()) {
		throw ex_smartScan("Cannot start scan without reference points set.", __func__, __FILE__);
	}

	scans.at(scanId)->Run();
}

void SmartScanService::ClearData()
{
	mDataAck.Stop(true);

	for (int i = 0; i < scans.size(); i++) {
		scans.at(i)->Stop(true);
	}
}

Point3 SmartScanService::GetSingleSample(int sensorSerial)
{
	return mDataAck.getSingleSample(sensorSerial);
}

void SmartScanService::StopScan()
{
	mDataAck.Stop();

	for (int i = 0; i < scans.size(); i++) {
		scans.at(i)->Stop();
	}
}

void SmartScanService::StopScan(int scanId)
{
	if (!IdExists(scanId)) {
        throw ex_smartScan("Scan id does not exist", __func__, __FILE__);
	}

	scans.at(scanId)->Stop();
}

// TODO: Optionally handle multiple simultaneous scans (i.e. some processing is being done on a previous scan
// while new data is acquired usign a different Scan object"
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

const int SmartScanService::NumAttachedBoards() const
{
	return mDataAck.NumAttachedBoards();
}

const int SmartScanService::NumAttachedTransmitters() const
{
	return mDataAck.NumAttachedTransmitters();
}

const int SmartScanService::NumAttachedSensors(bool includeRef) const
{
	return mDataAck.NumAttachedSensors(includeRef);
}

void SmartScanService::ExportCSV(const std::string filename, int scanId, const bool raw)
{
	try {
		if (raw) {
			csvExport.ExportPoint3Raw(mDataAck.getRawBuffer(), filename);
		}
		else {
			std::vector<Point3> temp;
			scans.at(scanId)->CopyOutputBuffer(&temp);
			csvExport.ExportPoint3(&temp, filename);
		}
	}
	catch(ex_export e) {
		throw e;
	}
	catch (...) {
		throw ex_smartScan("Could not export data.", __func__, __FILE__);
	}
}

void SmartScanService::ExportPointCloud(const std::string filename, int scanId, const bool raw)
{
	try {
		if (raw) {
			csvExport.ExportPoint3RawCloud(mDataAck.getRawBuffer(), filename);
		}
		else {
			std::vector<Point3> temp;
			scans.at(scanId)->CopyOutputBuffer(&temp);
			csvExport.ExportPoint3Cloud(&temp, filename);
		}
	}
	catch(ex_export e) {
		throw e;
	}
	catch (...) {
		throw ex_smartScan("Could not export data.", __func__, __FILE__);
	}
}

void SmartScanService::RegisterRawDataCallback(std::function<void(const std::vector<SmartScan::Point3>&)> callback)
{
	// Register this callback with all the existing Scans:
	mDataAck.RegisterRawDataCallback(callback);
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