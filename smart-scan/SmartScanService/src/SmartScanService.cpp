#include <chrono>
#include <iomanip>

#include "../inc/Exceptions.h"
#include "../inc/SmartScanService.h"
#include "../inc/DataAcquisition.h"

using namespace SmartScan;

SmartScanService::SmartScanService(bool useMockData) : mUseMockData{ useMockData }, mDataAcq(useMockData) {  // Initializer list needed to initialize member classes and values.

}

SmartScanService::~SmartScanService() {
	// Clear all data.
	this->ClearData();
	scans.clear();
}

void SmartScanService::Init() {
	mDataAcq.Init();
}

void SmartScanService::Init(DataAcqConfig acquisitionConfig) {
	mDataAcq.Init(acquisitionConfig);
}

void SmartScanService::CorrectZOffset(int serialNumber) {
	if (mUseMockData) {
		return;
	}
	mDataAcq.CorrectZOffset(serialNumber);
}

void SmartScanService::NewScan(ScanConfig config) {
	// Check if the filtering precision has no remainder with 180. This is needed to make sure an array can be created with integer indexes in the Scan class.
	if (180%config.filteringPrecision != 0) {
		throw ex_smartScan("180 is not a multiple of the filtering precision", __func__, __FILE__);
	}
	// Give raw data buffer to the scan.
	config.inBuff = mDataAcq.GetRawBuffer();
	this->scans.emplace_back(std::make_shared<Scan>(FindNewScanId(), config));
}

void SmartScanService::DeleteScan() {
	this->scans.clear();
}

void SmartScanService::DeleteScan(int id) {
	bool ok = false;
	// Find the scan with the specified id and erase it.
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

void SmartScanService::StartScan() {
	char arg[51];
	const char startString[] = "Cannot start scan ";
	const char endString[] = " due to reference points set.";
	char scan[3];

	// Start the scan:
	mDataAcq.Start();

	for (int i = 0; i < scans.size(); i++) {
		// Check if reference points have been set;
		if (!this->scans.at(i)->NumRefPoints() && !this->scans.at(i)->NumUsedSensors()) {
			// Ugly stuff to put a number into an error string. This can probalby be done easier.
			sprintf_s(scan, "%d", i);
			strcpy_s(arg, startString);
			strcat_s(arg, scan);
			strcat_s(arg, endString);
			throw ex_smartScan(arg, __func__, __FILE__);
		}

		scans.at(i)->Run();
	}
}

void SmartScanService::ClearData() {
	mDataAcq.Stop(true);

	for (int i = 0; i < scans.size(); i++) {
		scans.at(i)->Stop(true);
	}
}

Point3 SmartScanService::GetSingleSample(int sensorSerial, bool raw) {
	return mDataAcq.GetSingleSample(sensorSerial, raw);
}

void SmartScanService::StopScan() {
	mDataAcq.Stop();

	for (int i = 0; i < scans.size(); i++) {
		scans.at(i)->Stop();
	}
}

const std::vector<std::shared_ptr<Scan>>& SmartScanService::GetScansList() const {
	return scans;
}

const int SmartScanService::NumAttachedBoards() const {
	return mDataAcq.NumAttachedBoards();
}

const int SmartScanService::NumAttachedTransmitters() const {
	return mDataAcq.NumAttachedTransmitters();
}

const int SmartScanService::NumAttachedSensors(bool includeRef) const {
	return mDataAcq.NumAttachedSensors(includeRef);
}

void SmartScanService::ExportCSV(const std::string filename, int scanId, const bool raw) {
	try {
		if (raw) {
			csvExport.ExportPoint3Raw(mDataAcq.GetRawBuffer(), filename);
		}
		else {
			// Copy the scan buffer into a temporary vector and export that.
			// This is done because the scan data is stored in a 3D array so its not that nice to work with.
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

void SmartScanService::ExportPointCloud(const std::string filename, int scanId, const bool raw) {
	try {
		if (raw) {
			csvExport.ExportPoint3RawCloud(mDataAcq.GetRawBuffer(), filename);
		}
		else {
			// Copy the scan buffer into a temporary vector and export that.
			// This is done because the scan data is stored in a 3D array so its not that nice to work with.
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

void SmartScanService::RegisterRawDataCallback(std::function<void(const std::vector<SmartScan::Point3>&)> callback) {
	mDataAcq.RegisterRawDataCallback(callback);
}

const int SmartScanService::FindNewScanId() const {
	int newId = 0;
	for (int scn = 0; scn < scans.size(); scn++) {
		if (newId == scans[scn]->mId) {
			++newId;
		}
	}

	return newId;
}

const bool SmartScanService::IdExists(const int scanId) const {
	for (int scn = 0; scn < scans.size(); scn++) {
		if (scanId == scans[scn]->mId) {
			return true;
		}
	}
	return false;
}