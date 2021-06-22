#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <chrono>
#include <functional>

#include "Point3.h"
#include "Filtering.h"
#include "TrakStarController.h"
#include "Trigger.h"

namespace SmartScan
{
	class Scan
	{
	public:
		const int mId;                                  			// Scan identifier.

		Scan(const int id, std::vector<std::vector<Point3>*> rawDataBuffer, std::vector<Point3> refPoints, int filteringPrecision);

		~Scan();

		void Run();

		void Stop(bool clearData = false);

		const bool IsRunning() const;

		const int NumUsedSensors() const;

		const int NumRefPoints() const;

		const double GetFilteringPrecision() const;
	private:
		bool mRunning = false;                          			// Indicates if scan is running.

		const std::vector<std::vector<Point3>*> mInBuff;    		// Raw data vector.
		const std::vector<Point3> mRefPoints;              			// Reference point vector.
		const int mFilteringPrecision;							// Filtering precision.

		std::vector<std::vector<std::vector<Point3>>> mSortedBuff;	// Vector containing ther sorted points.

		std::unique_ptr<std::thread> pScanningThread;
		
		void DataFiltering();
	};
}