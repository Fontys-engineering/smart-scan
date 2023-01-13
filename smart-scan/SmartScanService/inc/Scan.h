// This is the SmartSCan Scan class.
// It provides an interface for creating foot scans out of the clinician's movements.
// This is done by using the "gradient smoothing" algorithm from the S7 group (2020).

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
		int stopAtSample = -1;										// Stop scanning after a certain sample is reached.
		float outlierThreshold = 1000000;							// Do not store points if their distance from lower data points are larger than this value.
    };

	class Scan
	{
	public:
		const int mId;                                  			// Scan identifier.

		// Constructor. Creates a Scan object that filters the clinician's movements into a foot scan.
		// Arguments:
		// - id : Unique scan identifier. Used for exporting and deleting individual scans. 
		// - config : Configuration options of this particular scan.
		Scan(const int id, ScanConfig config);

		// Destructor. Is here to make sure the data is cleaned up if the Scan object is removed.
		~Scan();

        // Start a Scan thread that will continuously filter the raw buffer values into a sorted buffer.
		void Run();

        // Stop the Scan thread. It will continue to filter until it either reached the stopAtSample value or the raw buffer size.
        // Arguments: 
		// - clearData : When set to "True", it will erase all recorded data after stopping the thread.
		void Stop(bool clearData = false);

        // Returns a boolean indicating if the Scan thread is running.
		const bool IsRunning() const;

		// Copies the sorted buffer into a single vector.
		// Arguments:
		// - buffer : pointer to a Point3 vector in which the sorted buffer needs to be copied.
		void CopyOutputBuffer(std::vector<Point3>* buffer) const;

		// Returns the number of sensors in the raw data buffer.
		const int NumUsedSensors() const;

		// Returns the number of reference points defined in the configuration options.
		const int NumRefPoints() const;

		// Returns the filtering precision defined in the configuration options.
		const int GetFilteringPrecision() const;

		// Returns the stopAtSample parameter defined in the configuration options.
		const int GetStopAtSample() const;

		// Returns the stopAtSample parameter defined in the configuration options.
		void SetStopAtSample(int newStopSample);

		// Returns the outlier threshold parameter defined in the configuration options.
		const double GetOutlierThreshold() const;

		//BROKEN FILTERING, MIGHT DATA OVERFLOW, ISSUE: FILTERS ALL POINTS TO REF POINT
		// Filters out artifacts created when the point cloud is not dense enough
		//void OutlierFiltering(void);

	private:
		const double pi = 3.141592653589793238463;					// Approximation of PI.
		const float toAngle = 180/pi;								// Radian to Degree conversion.

		bool mRunning = false;										// Boolean indicating if the scan thread is running.

		ScanConfig mConfig;									// Scan configuration object.

		std::vector<std::vector<std::vector<Point3>>> mSortedBuff;	// Vector containing ther sorted points.

		int mLastFilteredSample = 0;								// Last filtered sample. Needed to know when to stop.

		std::unique_ptr<std::thread> pScanningThread;				// Scanning thread.
		
		// Function that filters the data from the raw buffer into a sorted array containing only the points on the foot.
		// This function is run in a seperate thread.
		void DataFiltering();

		// Returns the index of the nearest reference point and calculates the radius between the sensor point and this reference point.
		// Arguments:
		// - point : Pointer to the sensor data Point3 that needs to be evaluated. (Will fill in the radius in that point).
		int CalcNearestRef(Point3* point);

		// Calculates the theta and phi between two points and stores the result in point.
		// Arguments:
		// - refPoint : Reference point. Seen as the origin.
		// - point : Pointer to the sensor data Point3 in which the theta and phi parameters are stored. 
		void CalcAngle(Point3 refPoint, Point3* point);
	};
}