#pragma once

#include <time.h>
#include <string>
#include <fstream>
#include <vector>

#include "Point3.h"
#include "NDI/ATC3DG.h"

namespace SmartScan
{
	class CSystem
	{
	public:
		SYSTEM_CONFIGURATION	m_config;
	};

	class CSensor
	{
	public:
		SENSOR_CONFIGURATION	m_config;
	};

	class CXmtr
	{
	public:
		TRANSMITTER_CONFIGURATION	m_config;
	};

    // This class handles the communication with the TrakSTAR hardware
    // Functionality such as correct configuration, starting, stopping the scan and
    // Getting new data is handled here.
	class TrakStarController
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="mock">when true, the TrakStarController will not attempt to use hardware TrakStar and use Mock data instead</param>
		/// <returns></returns>
		TrakStarController(bool mock = false);
		~TrakStarController();

		/// <summary>
		/// Initialie the system. Call this before making a measurement
		/// </summary>
		void Init();

		/// <summary>
		/// Turn off the transmitter.
		/// </summary>
		void StopTransmit();

		void SelectTransmitter(short int id);

		void SetPowerlineFrequency(double freq);

		void SetMeasurementRate(double freq);

		void SetMaxRange(double range);

		void SetMetric();

		void SetSensorFormat();

        /// <summary>
        /// Get the port numbers of the attached sensors.
        /// </summary>
		std::vector<int> GetAttachedPorts();
		std::vector<int> GetAttachedSerials();

		/// <summary>
		/// Get the latest record for a specific sensor
		/// </summary>
		/// <param name="sensorID"> - The ID of the sensor from which the record will be returned</param>
		/// <returns> - The sensor record</returns>
		Point3 GetRecord(int sensorID);

	private:
		const bool mUseMockData;

		std::string s0MockDataFilePath = "MockData/s0.csv";
		std::string s1MockDataFilePath = "MockData/s1.csv";
		std::string s2MockDataFilePath = "MockData/s2.csv";

		std::ifstream s0MockDataFile;
		std::ifstream s1MockDataFile;
		std::ifstream s2MockDataFile;

		long mockDataFileLine = 0;
		long mockDataFileNOfLines;

		CSystem	ATC3DG;
		std::vector<CSensor> pSensor;
		std::vector<CXmtr> pXmtr;

		/// Keep track of the last mock record so that the movement is realistic.
		Point3 mPrevMockRecord;

		void DeviceStatusHandler(int deviceStatus);

		void ErrorHandler(int error);

		const std::string GetErrorString(int error);
		
		/// <summary>
		/// Randomly returns a point on the edge of a sphere at a reasonable distance from the previous point.
		/// </summary>
		/// <returns></returns>
		Point3 GetMockRecord();

		/// <summary>
		/// Goes through the specified file and returns consecutive samples as Point3.
		/// </summary>
		/// <returns></returns>
		Point3 GetMockRecordFromFile(int sensorId = 0);
		
	};
}