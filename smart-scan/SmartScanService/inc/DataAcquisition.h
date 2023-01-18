// This is the SmartSCan data acquisition class.
// It provides an abstraction layer for gathering and storing sensor data from the TrakStar device.

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
        double measurementRate = 255;                   // Between 20.0 and 255.0.
        double powerLineFrequency = 50.0;               // Either 50.0 or 60.0.
        double maximumRange = 36.0;                     // Either 36.0 (914,4 mm), 72.0 and 144.0.
		int refSensorSerial = -1;						// Serial number of the reference sensor, set as -1 when no reference sensor is used.
		int thumbSensorSerial = -1;
		int middleSensorSerial = -1;
		int indexSensorSerial = -1;
		double frameRotations[3] = {0, 0, 0};			// Set the rotation of the measurement frame, azimuth, elevation and roll. (0, 0, 0) is default.

		DataAcqConfig();
		DataAcqConfig(short int transmitterID, double measurementRate, double powerLineFrequency, double maximumRange, int refSensorSerial, double frameRotations[3]);
    };

	class DataAcq 
	{
	public:
		// Constructor. Creates a DataAcquisition object that handles data storage and a TrakStarController abstraction layer.
		// Arguments:
		// - useMockData : When set to "true", a mock trakSTAR device is used. It will try to use the real thing otherwise.
		DataAcq();

		// Destructor. Is here to make sure the data is cleaned up if the DataAcquisition object is removed.
		~DataAcq();

		// Initialise the SmartScan data acquisition. Call this before starting scans.
		// Arguments:
		// - acquistionConfig : Configuration struct that specifies the settings with which the TrakStar device is initalized. 
		void Init();
		void Init(DataAcqConfig acquisitionConfig);

		// Assign the connected sensor IDs 
		// Arguments:
		// - refer : reference sensor IDs
		// - thumb : thumb sensor IDs
		// - index : index finger sensor IDs
		// - middle : middle finger sensor IDs
		void SensorConfig(int refer, int thumb, int index, int middle);

		// Set the Z offset of a specifc sensor. This is needed to compensate for the sensor being put on top of the fingers.
		// Arguments:
		// - serialNumber : Serial number of the sensor where the offset will be changed.
		void CorrectZOffset(int serialNumber);

        // Start a DataAcquisition thread that will continuously record values into a buffer.
		void Start();

        // Stop the DataAcquisition thread.
        // Arguments: 
		// - clearData : When set to "True", it will erase all recorded data after stopping the thread.
		void Stop(bool clearData = false);

        // Returns a boolean indicating if the DataAcquisition thread is running.
		const bool IsRunning() const;

        // Returns a pointer to the raw data buffer, for read-only access.
		const std::vector<std::vector<Point3>>* GetRawBuffer();

		// Acquire a single sample from a specific sensor.
		// Returns a Point3 object.
		// Arguments: 
		// - sensorNumber : Serial number of the sensor to get sample from.
		// - raw : When set to "True", the acquired sample will not be corrected for the reference sensor.
		Point3 GetSingleSample(int serialNumber, bool raw);

		// Returns the number of attached boards to this PC.
		const int NumAttachedBoards() const;

		// Returns the number of attached transmitters to the TrakStar device.
		const int NumAttachedTransmitters() const;

		// Returns the number of attached transmitters to the TrakStar device.
		const int NumAttachedSensors(bool includeRef) const;

		// Returns all connected serial numbers
		const std::vector<int> GetSerialNumbers(void);

		// Register a new callback function to be called whenever new raw data is available.
		// Arguments:
		// - callback : Contains the function that is executed. The function should take a vector of points as an argument.
		void RegisterRawDataCallback(std::function<void(const std::vector<Point3>&)> callback);
	private:
		DataAcqConfig mConfig;                    							// DataAcquisition configuration obj.

		bool mRunning = false;                          					// Boolean indicating if the DataAcquisition thread is running.

		TrakStarController mTSCtrl;                     					// TrackStar controller obj.
		Trigger button_obj;													// Trigger obj.

		int refSensorPort = -1;												// Port number of the reference sensor.
		int thumbSensorPort = -1;											// Port number of the thumb sensor.
		int middleSensorPort = -1;											// Port number of the middle finger sensor.
		int indexSensorPort = -1;											// Port number of the index sensor.
		std::vector<int> mPortNumBuff;										// Vector containing the sensor port numbers.
		std::vector<int> mSerialBuff;										// Vector containing sensor serial numbers.
		std::vector<std::vector<Point3>> mRawBuff;      					// Raw data vector.

		std::unique_ptr<std::thread> pAcquisitionThread;					// Data acquisition thread.
		
		std::function<void(const std::vector<Point3>&)>mRawDataCallback;    // Callback for printing raw data in real time.

		const double toRad = 3.14159265/180;								// Store degree to rad constant for easier acces later.
        const double zCaseOffset = 45.72;                                   // Distance from bottom face of the transmitter to zero point.

		// Return the port number of a sensor based on its serial number. 
		// Arguments:
		// - serialNumber : Serial number of the sensor of which the port number is desired.
		int FindPortNum(int serialNumber);

		// Return the raw buffer index of a sensor based on its serial number. 
		// Arguments:
		// - serialNumber : Serial number of the sensor of which the raw buffer index is desired.
		int FindBuffNum(int serialNumber);

		// Function that gathers all sensor data and corrects it for the reference sensor if specified.
		// This function is run in a seperate thread.
		void DataAcquisition();

		// Correct a point for the rotation of a refernce sensor.
		// Arguments:
		// - refPoint : Pointer to the Point3Ref containing the position and rotation of the reference sensor.
		// - sensorPoint : Pointer to the Point3 containing the position and rotation of the sensor that needs correcting.
		void ReferenceCorrect(Point3Ref* refPoint, Point3* sensorPoint);

		// Rotate a point along itself. (Experimental function, only used in ZOffset calibration)
		// Arguments:
		// - sensorPoint : Pointer to the Point3 of the sample that needs to be rotated.
		void AngleCorrect(Point3* sensorPoint);
	};
}