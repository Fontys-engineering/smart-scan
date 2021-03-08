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
	class Scan
	{
	public:
		const int mId;                                  // Scan identifier.

		std::vector<Point3> mInBuff;                    // Raw data vector.
		std::vector<Point3> mOutBuff;                   // Filtered data vector.
		std::vector<Point3> mRefBuff;                   // Reference sensor data vector.

		Scan(const int id, TrakStarController* pTSCtrl);
		Scan(const int id, TrakStarController* pTSCtrl, const double sampleRate, const std::vector<int> usedSensors, const int refSensorId);
		~Scan();

		void Run(bool acqusitionOnly = false);

		void Stop(bool clearData = false);

		/// <summary>
		/// Register a new callback function to be called whenever new filtered data is available
		/// </summary>
		/// <param name="callback"> - the efunction to be called back.</param>
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
		const bool isRunning() const;

		//double GetCompletion() const;
		//bool Complete() const;
		//void PostProcess();

		/// <summary>
		/// Dumps all samples for the inBuffer to the console
		/// </summary>
		void DumpData() const;

		void SetSampleRate(const double sampleRate);

		const double GetSampleRate() const;
		
		//void SetUsedSensors();
		//void SetUsedSensors(const std::vector<int> usedSensors);

		const std::vector<int> GetUsedSensors() const;

		//void SetReferenceSensorId(const int sensorId);

		const int GetReferenceSensorId();

		/// <summary>
		/// return the number of sensors used for the measurement. Excluding the reference sensor(s)
		/// </summary>
		/// <returns> - number of sensors used</returns>
		const int NUsedSensors() const;

		void SetFilteringPrecision(const double precision);

		const double GetFilteringPrecision();

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

		const std::vector<int> mUsedSensors;	        // The sensors ids that we want a reading from.
		const int mRefSensorId;                         // Reference sensor id.
		double sampleRate = 50;	                        // Sample rate of the data acquisition in Hz.
		double mFilteringPrecision = 30;                // Filter precision angle from reference point.

		std::vector<ReferencePoint> mReferencePoints;

		TrakStarController *pTSCtrl;                    // Pointer to Track star controller obj

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

		void DataFiltering();
	};
}