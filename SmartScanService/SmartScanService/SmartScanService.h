#pragma once
//This is the main smart scan library class
//It makes use of the Filtering, TrackStarController, Trigger and CSVExport sub-classes
//
//Applications using SmartScan software should use this class and not the other classes directly.

#include "Point3.h"
#include "ReferencePoint.h"
#include "Scan.h"
#include "ScanDb.h"
#include "TrakStarController.h"
#include "CSVExport.h"

#include <functional>
#include <vector>
#include <thread>
#include <chrono>

namespace SmartScan
{
	class SmartScanService
	{
	public:
		/// <summary>
		/// Cosntructor. Creates a SmartScanService object that handles all the functionality. 
		///	When debugging and the TrakSTAR device is unnavailable, set "useMockData" to "true". Otherwise,
		///	the TrakStarController will fail to initialise and throw errors.
		/// </summary>
		/// <param name="useMockData"> - When set to "true", a mock trakSTAR device is used.</param>
		SmartScanService(bool useMockData = false);
		~SmartScanService();

		/// <summary>
		/// Initialise the Smart Scan service, Trak star device etc. Call this after the TrakStar system is pluged-in
		/// </summary>
		void Init();

		/// <summary>
		/// Routine for calibrating the position of the sensors relative to the fingertips
		/// </summary>
		//void CalibrateSensors();

#pragma region scan

		/// <summary>
		/// Clears all existing references for the last scan and starts 
		/// the reference calibration routine.
		/// </summary>
		void CalibrateReferencePoints();

		/// <summary>
		/// When called, it sets a single reference point between the thub and index finger
		/// No CMDL interaction required
		/// </summary>
		void CalibrateSingleRefPoint();
		/// <summary>
		/// Set the reference points for the latest scan object:
		/// </summary>
		/// <param name="referencePoints"> - vector of reference points</param>
		void SetReferencePoints(const std::vector<ReferencePoint> referencePoints);
		/// <summary>
		/// Creates an empty new scan.
		/// </summary>
		/// <param name="sensorIds"> - if specified, only the sensors with the ids in this list will be used</param>
		void NewScan(const std::vector<int> sensorIds = {});

		void NewScan(const std::vector<int> sensorIds, const int refSensorId, const double sampleRate = 50);

		void NewScan(const int scanIdconst, std::vector<int> sensorIds, const int refSensorId, const double sampleRate = 50);
		
		/// <summary>
		/// Creates a new scan with a specific ID (unique). Throws an error if the id is already used
		/// </summary>
		/// <param name="scanId"> - unique scan id</param>
		/// <param name="sensorIds"> - [optional] which sensors are used for the scan</param>
		void NewScan(int scanId, const std::vector<int> sensorIds = {});
		/// <summary>
		/// Deletes the latest scan.
		/// </summary>
		void DeleteScan();
		/// <summary>
		/// Deletes the scan with the specified id.
		/// </summary>
		/// <param name="id"> - The id of the scan to be deleted</param>
		void DeleteScan(int id);

		/// <summary>
		/// Start the latest scan using only the specified sensors. If no scan exists, it will create a new one.
		/// </summary>
		/// <param name="sensorIds"> - vector of sensor ids. TrakSTAR ids start at 0</param>
		void StartScan(const std::vector<int> sensorIds = {});

		/// <summary>
		/// Start the scan with the specified ID.
		/// </summary>
		/// <param name="scanId"> - id of the scan to start</param>
		/// <param name="sensorIds"> - [optional] which sensors will be used for the scan</param>
		void StartScan(int scanId, const std::vector<int> sensorIds = {});
		/// <summary>
		/// Stop the latest scan
		/// </summary>
		void StopScan();
		/// <summary>
		/// print the values from the latest scan to console. Debugging only. Not recommended.
		/// </summary>
		void DumpScan() const;

		/// <summary>
		/// Specify which sensors are in use for the latest scan;
		/// </summary>
		/// <param name="sensorIds"> - vector of sensor ids. TrakSTAR ids start at 0</param>
		void SetUsedSensors(const std::vector<int> sensorIds);

		void GetSensorInformation();

		void SetFilteringPrecision(const double precision);

		/// <summary>
		/// Export the Point3 array in a csv format. Contains rotation.
		/// </summary>
		/// <param name="filename"> - the name of the output file</param>
		/// <param name="raw"> - when true, the outptu is the raw buffer. default = false</param>
		void ExportCSV(const std::string filename, const bool raw = false);
		/// <summary>
		/// Export the filtered Point3 array in a Point Cloud format (only x,y,z)
		/// </summary>
		/// <param name="filename"> - the name of the output file</param>
		/// <param name="raw"> - when true, the outptu is the raw buffer. default = false</param>
		void ExportPointCloud(const std::string filename, const bool raw = false);

		/// <summary>
		/// Register a new callback function to be called whenever new filtered data is available.
		/// The filtered data vector is provided through a reference as a parameter of the callback function.
		/// </summary>
		/// <param name="callback"> - the efunction to be called back.</param>
		void RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback);
		/// <summary>
		/// Register a new callback function to be called whenever new raw data is available.
		/// The raw data vector is provided through a reference as a parameter of the callback function.
		/// </summary>
		/// <param name="callback"></param>
		void RegisterRawDataCallback(std::function<void(std::vector<Point3>&)> callback);

		//TODO: Optionally handle multiple simultaneous scans (i.e. some processing is being done on a previous scan
		// while new data is acquired usign a different Scan object"

		/// <summary>
		/// Get a pointer to the latest scan object. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		/// </summary>
		/// <returns> - The latest scan (could be one still in progress)</returns>
		const std::shared_ptr<Scan> GetScan() const;
		/// <summary>
		/// Get a pointer to a specific scan object. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		/// </summary>
		/// <param name="id"> - the id of the desired scan (id is different from index)</param>
		/// <returns> - The scan with the given id (could be one still in progress)</returns>
		const std::shared_ptr<Scan> GetScan(int id) const;

		/// <summary>
		/// Get a list of all the scan objects. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		/// </summary>
		/// <returns> - A reference to the vector of Smart scan object pointers </returns>
		const std::vector<std::shared_ptr<Scan>>& GetScansList() const;

#pragma endregion
	
	private:
		bool mUseMockData;

		//this vector stores the current scan objects. Once we are done with a scan we should remove it to free up memory.
		std::vector<std::shared_ptr<Scan>> scans;

		//TODO: handle older scans
		//the scan files database object:
		//ScanDb scanDb;

		//trakstarcontroller obj
		TrakStarController *tSCtrl;

		//csv eport:
		CSVExport csvExport;

		//UI callback:
		std::function<void(std::vector<Point3>&)> mUICallback;
		//raw callback:
		std::function<void(std::vector<Point3>&)> mRawCallback;

		//calibration and configuration:
		int mThumbSensorId = 0;
		int mIndexSensorId = 1;
		const double refSetTime = 5000;		//time in milliseconds after which the point is considered a reference.
		const double tError = 20;			//tolerated translation error in mm
		const double rError = 20;			//tolerated rotation error in mm

		double mFilteringPrecision = 4;

		/// <summary>
		/// Looks at the scans and returns the first unused id;
		/// </summary>
		/// <returns> - unique, unused, id</returns>
		const int FindNewScanId() const ;

		/// <summary>
		/// check if a scan with the same id already exists
		/// </summary>
		/// <param name="scanId"> - the id we are checking for</param>
		/// <returns> - true if a scan with the same id exists</returns>
		const bool IdExists(const int scanId) const;\
		std::vector<int> usedSensors;
		unsigned int refSensorId;
	};
}