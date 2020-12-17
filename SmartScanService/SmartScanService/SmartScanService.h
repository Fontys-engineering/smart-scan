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

		SmartScanService(bool useMockData = false);
		~SmartScanService();

		/// <summary>
		/// Initialise the Smart Scan service, Trak star device etc.
		/// </summary>
		void Init();

		/// <summary>
		/// Routine for calibrating the position of the sensors relative to the fingertips
		/// </summary>
		//void CalibrateSensors();

#pragma region scan

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
		/// Register a new callback function to be called whenever new filtered data is available
		/// </summary>
		/// <param name="callback"> - the efunction to be called back.</param>
		void RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback);

		//Scan& GetScan() const;
		//Scan& GetScan(int id);
#pragma endregion
	
	private:
		bool mUseMockData;

		//this vector stores the current scan objects. Once we are done with a scan we should remove it to free up memory.
		std::vector<Scan> scans;

		//the scan files database object:
		//ScanDb scanDb;

		//trakstarcontroller obj
		TrakStarController *tSCtrl;

		//csv eport:
		CSVExport csvExport;

		//UI callback:
		std::function<void(std::vector<Point3>&)> mUICallback;
	};
}