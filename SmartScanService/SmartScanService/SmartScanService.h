#pragma once
//This is the main smart scan library class
//It makes use of the Filtering, TrackStarController, Trigger and CSVExport sub-classes
//
//Applications using SmartScan software should use this class and not the other classes directly.


#include "Point3.h";
#include "ReferencePoint.h";
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
		/// the reference callibration routine.
		/// </summary>
		void CalibrateReferencePoints();

		/// <summary>
		/// Start a new scan
		/// </summary>
		void StartScan();
		/// <summary>
		/// Stop the latest scan
		/// </summary>
		void StopScan();
		/// <summary>
		/// print the values from the latest scan to console. Debugging only. Not recommended.
		/// </summary>
		void DumpScan() const;
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

		//TODO: Optionally handle multiple simultaneous scans (i.e. some processing is being done on a previous scan
		// while new data is acquired usign a different Scan object"

		/// <summary>
		/// Get a reference to the latest scan object. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		/// </summary>
		/// <returns> - The latest scan (could be one still in progress)</returns>
		const Scan& GetScan() const;
		/// <summary>
		/// Get a reference to a specific scan object. Returned as const so no changes can be made to it. This is meant mostly for accessing the data.
		/// </summary>
		/// <param name="id"> - the id of the desired scan (id is different from index)</param>
		/// <returns> - The scan with the given id (could be one still in progress)</returns>
		const Scan& GetScan(int id) const;
#pragma endregion
	
	private:
		bool mUseMockData;

		//this vector stores the current scan objects. Once we are done with a scan we should remove it to free up memory.
		std::vector<Scan> scans;

		//TODO: handle older scans
		//the scan files database object:
		//ScanDb scanDb;

		//trakstarcontroller obj
		TrakStarController *tSCtrl;

		//csv eport:
		CSVExport csvExport;

		//UI callback:
		std::function<void(std::vector<Point3>&)> mUICallback;

		//calibration and configuration:
		int mThumbSensorId = 0;
		int mIndexSensorId = 1;
	};
}