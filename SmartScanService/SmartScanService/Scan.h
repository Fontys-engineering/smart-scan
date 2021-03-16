#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <chrono>
#include <functional>

#include "Point3.h"
#include "ReferencePoint.h"
#include "Filtering.h"
#include "TrakStarController.h"

namespace SmartScan
{
	struct ScanConfig
	{
		bool acquisitionOnly = true;                    // Only get raw sensor data, so no filtering.
		bool useReferenceSensor = false;                // Use a reference sensor to eliminate object movement during measurements.

		int referenceSensorId = 0;                      // Channel number of the reference sensor (when used).
		std::vector<int> usedSensorIds = { 1, 2 };      // Channel numbers of the non-reference sensors.

		double sampleRate = 50;                         // Set the data-acquisition rate.
		double filteringPrecision = 4;                  // Set the filtering angle width from reference point(s).
	};

	class Scan
	{
	public:
		const int mId;                                  // Scan identifier.

		std::vector<Point3> mInBuff;                    // Raw data vector.
		std::vector<Point3> mOutBuff;                   // Filtered data vector.
		std::vector<Point3> mRefBuff;                   // Reference sensor data vector.

		/// <summary>
		/// Scan constructor. Will use the default scan configuration.
		/// </summary>
		/// <param name="id"> - Scan unique identifier.</param>
		/// <param name="pTSCtrl"> - Pointer to a trackstar controller object.</param>
		Scan(const int id, TrakStarController* pTSCtrl);
		/// <summary>
		/// Scan constructor.
		/// </summary>
		/// <param name="id"> - Scan unique identifier.</param>
		/// <param name="pTSCtrl"> - Pointer to a trackstar controller object.</param>
		/// <param name="config"> - ScanConfig struct which contains the scans settings.</param>
		Scan(const int id, TrakStarController* pTSCtrl, const ScanConfig config);

		~Scan();

		/// <summary>
		/// Start a scan. This will always start a data acquisition thread but depending on the
		/// settings it will also start a filtering thread.
		/// </summary>
		/// <param name="acquisitionOnly"> - Force starting the data acquisition thread only.</param>
		void Run(bool acquisitionOnly = false);

		/// <summary>
		/// Stop a scan. This will stop the data acquisition thread as well as the filtering thread.
		/// </summary>
		/// <param name="clearData"> - Force starting the data acquisition thread only.</param>
		void Stop(bool clearData = false);

		/// <summary>
		/// Register a new callback function to be called whenever new filtered data is available
		/// </summary>
		/// <param name="callback"> - the eunction to be called back.</param>
		void RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback);

		/// <summary>
		/// Register a new callback function to be called whenever new raw data is available
		/// </summary>
		/// <param name="callback"> - the efunction to be called back.</param>
		void RegisterRawDataCallback(std::function<void(std::vector<Point3>&)> callback);

		/// <summary>
		/// return the status of the scan
		/// </summary>
		/// <returns> - status (true if the scan is running)</returns>
		const bool IsRunning() const;

		const bool IsAcquisitionOnly() const;

		/// <summary>
		/// Dumps all samples for the inBuffer to the console
		/// </summary>
		void DumpData() const;

		const int GetReferenceSensorId();

		const std::vector<int> GetUsedSensors() const;

		const double GetSampleRate() const;

		const double GetFilteringPrecision();

		/// <summary>
		/// return the number of sensors used for the measurement. Excluding the reference sensor(s)
		/// </summary>
		/// <returns> - number of sensors used</returns>
		const int NUsedSensors() const;

		/// <summary>
		/// Routine for finding the reference points required for the filtering algorithm
		/// </summary>
		void AddReference(const ReferencePoint ref);

		/// <summary>
		/// get a list of the existing reference points
		/// </summary>
		/// <returns> - vector containing the reference points</returns>
		const std::vector<ReferencePoint>& GetReferences() const;

		/// <summary>
		/// Delete all existing reference points
		/// </summary>
		void ResetReferences();
	private:
		bool mRunning = false;                          // Indicates if scan is running.
		bool mStopDataAcquisition = false;              // Stops data acquisition thread when true.
		bool mStopFiltering = false;                    // Stops filtering thread when true.

		const ScanConfig mConfig;                       // Scan configuration struct
		TrakStarController* pTSCtrl;                    // Pointer to Track star controller obj

		std::vector<ReferencePoint> mReferencePoints;

		Filtering mF;                                   // Filtering object.
		const unsigned int frameSize = 100;             // ?
		unsigned int frameCounter = 0;                  // ?
		unsigned long lastFilteredSample = 0;

		std::unique_ptr<std::thread> pAcquisitionThread;
		std::unique_ptr<std::thread> pFilteringThread;

		std::chrono::steady_clock::time_point lastSampleTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point scanStartTime = std::chrono::steady_clock::now();

		std::function<void(std::vector<Point3>&)> mNewDataCallback;     // ?New data callback
		std::function<void(std::vector<Point3>&)> mRawDataCallback;     // Needed for printing values directly to console 

		/// <summary>
		/// Polls the TrakstarController for new data, stores it and filters it.
		/// </summary>
		void DataAcquisition();

		/// <summary>
		/// Filters the input buffer and stores the result in the output buffer.
		/// </summary>
		void DataFiltering();
	};
}