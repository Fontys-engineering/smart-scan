// This is the main smart scan library class.
// It provides the interface with which the SmartScan device is controlled. 
// Applications using SmartScan software should use this class and not the other classes directly.

#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <chrono>

#include "Point3.h"
#include "Scan.h"
#include "DataAcquisition.h"
#include "CSVExport.h"

namespace SmartScan
{
	class SmartScanService
	{
	public:
		// Constructor. Creates a SmartScanService object that handles everything SmartScan related.
		// Arguments:
		SmartScanService();

		// Destructor. Is here to make sure the data is cleaned up if the SmartScan object is removed.
		~SmartScanService();

		// Initialise the SmartScan data acquisition. Call this before starting scans.
		// Arguments:
		// - acquistionConfig : Configuration struct that specifies the settings with which the TrakStar device is initalized. 
		void Init();
		void Init(DataAcqConfig acquisitionConfig);

		// Set the Z offset of a specifc sensor. This is needed to compensate for the sensor being put on top of the fingers.
		// Arguments:
		// - serialNumber : Serial number of the sensor where the Z offset will be changed.
		void CorrectZOffset(int serialNumber);

		// Creates a new scan and adds it to the scan list.
		// Arguments:
		// - config : Configuration struct that specifies the settings of this particular scan.
		void NewScan(ScanConfig config);

		// Deletes scans in the scan list.
		// Arguments:
		// - id : The id of the scan that needs to be deleted.
		void DeleteScan();
		void DeleteScan(int id);

		// Start the data acquistion and all the scans in the scan list. 
		void StartScan();

		// Clear all previous recorded data.
		void ClearData();

		// Acquire a single sample from a specific sensor.
		// Returns a Point3 object.
		// Arguments:
		// - sensorNumber : Serial number of the sensor to get sample from.
		// - raw : When set to "True", the acquired sample will not be corrected for the reference sensor.
		Point3 GetSingleSample(int serialNumber, bool raw = false);

		// Stop the data acquisition and with that all the scans. The scans will conitnue to filter until caught up with data acquisition.
		void StopScan();

		// Get a list of all the scan objects. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		// Returns a vector containing Scan objects by reference.
		const std::vector<std::shared_ptr<Scan>>& GetScansList() const;

		// Returns the number of attached boards to this PC.
		const int NumAttachedBoards() const;

		// Returns the number of attached transmitters to the TrakStar device.
		const int NumAttachedTransmitters() const;

		// Returns the number of attached sensors to the TrakStar device.
		// Arguments:
		// - includeRef : When set to "True", the returned number will include the reference sensor.
		const int NumAttachedSensors(bool includeRef) const;

		// Export the Point3 array in a csv format suited for the MATLAB Artificial intelligence scripts.
		// Arguments:
		// - filename : Name of the exported file.
		// - scanId : Id of the scan that needs to be exported. (Does nothing if exporting raw data)
		// - raw : When set to "True", the raw data (corrected for a reference sensor) will be exported instead.
		void ExportCSV(const std::string filename, int scanId, const bool raw = false);

		// Export the Point3 array in a csv format suited for Cloudcompare.
		// Arguments:
		// - filename : Name of the exported file.
		// - scanId : Id of the scan that needs to be exported. (Does nothing if exporting raw data)
		// - raw : When set to "True", the raw data (corrected for a reference sensor) will be exported instead.
		void ExportPointCloud(const std::string filename, int scanId, const bool raw = false);

		// Register a new callback function to be called whenever new raw data is available.
		// Arguments:
		// - callback : Contains the function that is executed. The function should take a vector of points as an argument.
		void RegisterRawDataCallback(std::function<void(const std::vector<Point3>&)> callback);
	private:

		DataAcq mDataAcq;								// Data acquisition obj.
		std::vector<std::shared_ptr<Scan>> scans;       // Vector containing all the scans. 

		CSVExport csvExport;                           	// CSVexport obj

		// Looks at the scan list and returns the first unused id.
		// Returns a unique, unused, id.
		const int FindNewScanId() const ;

		// Returns a boolean indicating if a scan with the same Id already exists.
		// Arguments:
		// - scanId : Id that needs to be checked.
		const bool IdExists(const int scanId) const;
	};
}