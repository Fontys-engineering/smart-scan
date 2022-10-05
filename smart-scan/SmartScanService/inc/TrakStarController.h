// This is the SmartScan TrakStarController class.
// Here the communication with the TrakSTAR hardware is handled for example, configration settings and acquiring samples.

#pragma once

#include <time.h>
#include <string>
#include <fstream>
#include <vector>

#include "Point3.h"
#include "ATC3DG.h"

namespace SmartScan
{
	// Class containing the system configuration.
	class CSystem {
	public:
		SYSTEM_CONFIGURATION	m_config;
	};

	// Class containing the transmitter configuration.
	class CXmtr {
	public:
		TRANSMITTER_CONFIGURATION	m_config;
	};

	// Class containing the sensor configuration.
	class CSensor {
	public:
		SENSOR_CONFIGURATION	m_config;
	};

	class TrakStarController {
	public:
		// Constructor. Creates a TrakStarController object that handles communication with the TrakStar device.
		// Arguments:
		// - mock : When set to "true", a mock trakSTAR device is used. It will try to use the real thing otherwise.
		TrakStarController(bool mock = false);

		// Destructor. Is here to make sure the transmitter is turned off when the TrakStar object is removed.
		~TrakStarController();

		// Initialize the system and acquires system, transmitter and sensor configuration. Call this before making a measurement.
		void Init();

		// Turn off the transmitter.
		void StopTransmit();

		// Select which transmitter the device is using. (Can potentially be multiple when daisy chaining)
		// Arguments:
		// - id : Id of the transmitter which is going to be used.
		void SelectTransmitter(short int id);

		// Set the powerline frequency.
		// Arguments:
		// - freq : Powerline frequency. Either 50.0 or 60.0.
		void SetPowerlineFrequency(double freq);

		// Set the measurement rate of the device in Hz.
		// Arguments:
		// - freq : Measurement rate of the device. Range between 20 and 255 Hz.
		void SetMeasurementRate(double freq);

		// Set the maximum range of the sensor X, Y and Z axis.
		// Arguments:
		// - range : Maximum range of the axis in inches. Can be either 36, 72 or 144.
		void SetMaxRange(double range);

		// Set the device to report coordinates in millimetres.
		void SetMetric();

		// Set the reference frame of the transmitter. Used to rotate the axis since they may not align with the transmitter casing.
		// Arguments:
		// - id : Id of the transmitter where the reference frame will be adjusted.
		// - angles : Array of 3 angles with which the axis are rotated. (Azimuth, Elevation, Roll)
		void SetReferenceFrame(short int id, double angles[3]);

		// Set all available sensors to use the DOUBLE_POSITION_ANGLES_TIME_Q_BUTTON format.
		void SetSensorFormat();

		// Set one sensor to use the DOUBLE_POSITION_MATRIX format. Used for reference sensor data since it requires rotation matrices.
		// Arguments:
		// - id : Port number of the sensor which its format needs to be changed.
		void SetRefSensorFormat(int id);

		// Set the X, Y and Z offset of a sensor. Used to correct for finger t h i c c n e s s.
		// Arguments:
		// - id : Port number of the sensor which its offset needs to be changed.
		// - offset : Point3 containing the desired X, Y and Z offsets.
		void SetSensorOffset(int id, Point3 offset);

		// Return the number of attached boards.
		const int NumAttachedBoards() const;

		// Return the number of attached transmitters.
		const int NumAttachedTransmitters() const;

		// Return a vector containing the sensor ports where an actual sensor is attached.
		std::vector<int> GetAttachedPorts() const;

		// Return a vector containing the sensor serial numbers of all attached sensors.
		std::vector<int> GetAttachedSerials() const;

		// Get the latest record for a specific sensor using the DOUBLE_POSITION_ANGLES_TIME_Q_BUTTON format.
		// Arguments:
		// - id : The ID of the sensor from which the record will be returned.
		Point3 GetRecord(int id);

		// Get the latest record for a specific sensor using the DOUBLE_POSITION_MATRIX format.
		// Arguments:
		// - id : The ID of the sensor from which the record will be returned.
		Point3Ref GetRefRecord(int id);
	private:
		const double toInch = 0.03937008;						// Constant for converting millimetres to inches.

		const bool mUseMockData;								// Boolean indicating if Mock data is used.	

		CSystem	ATC3DG;											// System configuration.
		std::vector<CXmtr> pXmtr;								// Vector of transmitter configurations.
		std::vector<CSensor> pSensor;							// Vector of sensor configurations.

		std::string s0MockDataFilePath = "MockData/s0.csv";		// File name of the first mock data file.
		std::string s1MockDataFilePath = "MockData/s1.csv";		// File name of the second mock data file.
		std::string s2MockDataFilePath = "MockData/s2.csv";		// File name of the third mock data file.

		std::ifstream s0MockDataFile;							// Input stream object for first mock data file.
		std::ifstream s1MockDataFile;							// Input stream object for second mock data file.
		std::ifstream s2MockDataFile;							// Input stream object for third mock data file.

		long mockDataFileLine = 0;								// Current mock data file line.
		long mockDataFileNOfLines;								// Total number of mock data file lines.

		// Keep track of the last mock record so that the movement is realistic.
		Point3 mPrevMockRecord;

		// Validate a device status and check which error has occurred if not valid.
		// Arguments:
		// - deviceStatus : The status value that needs to be checked.
		void DeviceStatusHandler(int deviceStatus);

		// Validate an error status and check which error has occurred if not valid.
		// Arguments:
		// - error : The error value that needs to be checked.
		void ErrorHandler(int error);

		// Similar to ErrorHandler() but returns a string of the error instead of throwing an exception.
		// Arguments:
		// - error : The error value that needs to be checked.
		const std::string GetErrorString(int error);
		
		// Randomly returns a point on the edge of a sphere at a reasonable distance from the previous point.
		Point3 GetMockRecord();

		// Goes through the specified file and returns consecutive samples as Point3.
		// Arguments:
		// -sensorId : Mock sensor port number.
		Point3 GetMockRecordFromFile(int sensorId = 0);
	};
}