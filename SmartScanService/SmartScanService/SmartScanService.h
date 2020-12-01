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

#include <vector>
#include <thread>
#include <chrono>

namespace SmartScan
{
	class SmartScanService
	{
	public:

		SmartScanService();

		/// <summary>
		/// Initialise the Smart Scan service, Trak star device etc.
		/// </summary>
		void Init();

		/// <summary>
		/// Routine for calibrating the position of the sensors relative to the fingertips
		/// </summary>
		//void CalibrateSensors();

#pragma region scan

		void StartScan();
		void StopScan();

		//Scan& GetScan() const;
		//Scan& GetScan(int id);
#pragma endregion
	
	private:
		//this vector stores the current scan objects. Once we are done with a scan we should remove it to free up memory.
		std::vector<Scan> scans;

		//the scan files database object:
		//ScanDb scanDb;

		//trakstarcontroller obj
		TrakStarController *tSCtrl;
	};
}