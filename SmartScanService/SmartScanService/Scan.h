#pragma once

#include "Point3.h"
#include "ReferencePoint.h"
#include "Filtering.h"
#include "TrakStarController.h"

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <chrono>
#include <functional>

namespace SmartScan
{
	class Scan
	{
	public:
		const int mId;
		/// <summary>
		/// raw data vector
		/// </summary>
		std::vector<Point3> mInBuff;
		/// <summary>
		/// Filtered data vector
		/// </summary>
		std::vector<Point3> mOutBuff;
		/// <summary>
		/// reference sensor data vector
		/// </summary>
		std::vector<Point3> mRefBuff;

		Scan(const int id, TrakStarController* pTSCtrl);

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

#pragma region configuration

		void SetSampleRate(const double sampleRate);
		const double GetSampleRate() const;
		void SetUsedSensors(const std::vector<int> usedSensors);
		void SetUsedSensors();

		/// <summary>
		/// return the number of sensors used for the measurement. Excluding the reference sensor(s)
		/// </summary>
		/// <returns> - number of sensors used</returns>
		const int NUsedSensors() const;

#pragma endregion configuration

#pragma region reference_points

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

#pragma endregion reference points:

	private:
#pragma region scan_properties
		double sampleRate = 50;	//in hz

#pragma endregion scan properties:

		std::vector<ReferencePoint> mReferencePoints;

		//track star controller obj:
		TrakStarController *pTSCtrl;

#pragma region data_acquisition
		//status:
		bool mRunning = false;

		//data acquisition thread:
		std::unique_ptr<std::thread> pAcquisitionThread;

		/// <summary>
		/// Polls the TrakstarController for new data, stores it and filters it.
		/// </summary>
		void DataAcquisition();
		bool mStopDataAcquisition = false;

		std::vector<int> mUsedSensors;	//the sensors ids that we want a reading from.
		int mRefSensorId = 0;

		//filtering thread:
		std::unique_ptr<std::thread> pFilteringThread;

		void DataFiltering();
		bool mStopFiltering = false;

		const unsigned int frameSize = 150;
		unsigned int frameCounter = 0;
		unsigned long lastFilteredSample = 0;

		//timing:
		std::chrono::steady_clock::time_point lastSampleTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point scanStartTime = std::chrono::steady_clock::now();


		//new data callback
		std::function<void(std::vector<Point3>&)> mNewDataCallback;
		//raw data callback
		std::function<void(std::vector<Point3>&)> mRawDataCallback;

#pragma endregion data aquisition:
	
		Filtering mF;
	};
}