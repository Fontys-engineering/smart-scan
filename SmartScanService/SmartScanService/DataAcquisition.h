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
		int refSensorSerial = -1;						// Serial number of the reference sensor, set as -1 when no reference sensor is used.
		bool useMatrix = true;							// Use rotation matrixes to correct for the reference sensor instead of euler angles.
    };

	class DataAcq 
	{
	public:
        // Data acquisition constructor.
        // If useMockData, is true it will not initalize a real trakStar device but use mockdata instead.
        // AcquisitionConfig contains the parameters with which the trakStar device is initialized.
		DataAcq(bool useMockData);
		~DataAcq();

        // Initialize data acquisition.
        // It will initialize the trakStar device using either the default paramaters or the user defined
        // ones if specifified in the constructor.
        // It will also retrieve sensor information and match ports and serial numbers.
		void Init();
		void Init(DataAcqConfig acquisitionConfig);

        // Start a data-acquisition thread that will continuously record values into a buffer.
		void Start();

        // Stop the data-acquisition thread.
        // If clearData is true, it will erase all recorded data after stopping the thread.
		void Stop(bool clearData = false);

        // Returns a boolean indicating if the data-acquisition thread is running.
		const bool IsRunning() const;

		Point3 getSingleSample(int sensorSerial);
        // Returns a pointer to the raw data buffer,for read-only access.
		const std::vector<Point3>* getSingleRawBuffer(int serialNumber);
		const std::vector<std::vector<Point3>>* getRawBuffer();

        // Returns the number of attached trakStar boards.
        // Returns 1 if mock data is used.
		const int NumAttachedBoards() const;

        // Returns the number of attached transmitters.
        // Returns 1 if mock data is used.
		const int NumAttachedTransmitters() const;

        // Returns the number of attached sensors.
        // Returns 3 if mock data is used.
		const int NumAttachedSensors(bool includeRef) const;

        // Register a raw callback that is given to the data-acquisition thread
        // for displaying measurements in real-time.
		void RegisterRawDataCallback(std::function<void(const std::vector<Point3>&)> callback);
	private:
		const bool mUseMockData;											// Mock data indicator
		DataAcqConfig mConfig;                    							// Scan configuration obj

		bool mRunning = false;                          					// Indicates if scan is running.

		TrakStarController mTSCtrl;                     					// TrackStar controller obj
		Trigger button_obj;													// Trigger obj

		int refSensorPort = -1;												// Port number of the reference sensor
		std::vector<int> mPortNumBuff;										// Vector containing the sensor port numbers
		std::vector<int> mSerialBuff;										// Vector containing sensor serial numbers
		std::vector<std::vector<Point3>> mRawBuff;      					// Raw data vector.

		std::unique_ptr<std::thread> pAcquisitionThread;					// Data acquisition thread
		
		std::function<void(const std::vector<Point3>&)>mRawDataCallback;    // Callback for printing raw data in real time

		const double toRad = 3.14159265/180;								// Store degree to rad constant for easier acces later.

		int findPortNum(int sensorSerial);
		int findBuffNum(int sensorSerial);

		void DataAcquisition();

		void referenceCorrect(Point3* refPoint, Point3* sensorPoint);
		void referenceCorrect(Point3Ref* refPoint, Point3* sensorPoint);
	};
}