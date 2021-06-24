#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <functional>
#include <cmath>

#include "Point3.h"

namespace SmartScan
{
    struct ScanConfig
    {
		const std::vector<std::vector<Point3>>* inBuff;    			// Raw data vector.
		std::vector<Point3> refPoints;              				// Reference point vector.
		int filteringPrecision;										// Filtering precision.
		int stopAtSample;											// Stop scanning after a certain sample is reached.
		float outlierThreshold;										// Do not store points if their distance from the reference points are larger than this value.
    };

	class Scan
	{
	public:
		const int mId;                                  			// Scan identifier.

		Scan(const int id, ScanConfig config);

		~Scan();

		void Run();

		void Stop(bool clearData = false);

		const bool IsRunning() const;

		void CopyOutputBuffer(std::vector<Point3>* buffer) const;

		const int NumUsedSensors() const;

		const int NumRefPoints() const;

		const int GetFilteringPrecision() const;

		const int GetStopAtSample() const;

		const double GetOutlierThreshold() const;
	private:
		bool mRunning = false;
		const ScanConfig mConfig;

		const double pi = 3.141592653589793238463;
		const float toAngle = 180/pi;

		std::vector<std::vector<std::vector<Point3>>> mSortedBuff;	// Vector containing ther sorted points.

		int mLastFilteredSample = 0;

		std::unique_ptr<std::thread> pScanningThread;
		
		void DataFiltering();

		int CalcNearestRef(Point3* point);
		void calcAngle(Point3 refPoint, Point3* point);
	};
}