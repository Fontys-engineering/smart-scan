#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <chrono>
#include <functional>
#include <cmath>

#include "Point3.h"
#include "TrakStarController.h"
#include "Trigger.h"

namespace SmartScan
{
    struct DataAcqConfig
    {
        short int transmitterID = 0;                    // Port of the transmitter, is usually 0 with one trakStar device.
        double measurementRate = 50;                    // Between 20.0 and 255.0
        double powerLineFrequency = 50.0;               // Either 50.0 or 60.0
        double maximumRange = 36.0;                     // Either 36.0 (914,4 mm), 72.0 and 144.0.
		int refSensorSerial = -1;						// Serial number of the reference sensor
    };

	class DataAcq 
	{
	public:
		DataAcq(bool useMockData);
		DataAcq(bool useMockData, DataAcqConfig acquisitionConfig);
		~DataAcq();

		void Init();

		void Start();

		void Stop(bool clearData = false);

		void RegisterRawDataCallback(std::function<void(const std::vector<Point3>&)> callback);

		const std::vector<std::vector<Point3>>* getRawBuffer();

		const bool IsRunning() const;

	private:
		const bool mUseMockData;											// Mock data indicator
		const DataAcqConfig mConfig;                    					// Scan configuration obj

		TrakStarController mTSCtrl;                     					// TrackStar controller obj
		Trigger button_obj;													// Trigger obj

		int refSensorPort;													// Port number of the reference sensor
		std::vector<int> mPortNumBuff;										// Vector containing the sensor port numbers
		std::vector<int> mSerialBuff;										// Vector containing sensor serial numbers
		std::vector<std::vector<Point3>> mRawBuff;      					// Raw data vector.

		bool mRunning = false;                          					// Indicates if scan is running.

		std::unique_ptr<std::thread> pAcquisitionThread;					// Data acquisition thread
		
		std::function<void(const std::vector<Point3>&)>mRawDataCallback;    // Callback for printing raw data in real time

		void DataAcquisition();
	};
}