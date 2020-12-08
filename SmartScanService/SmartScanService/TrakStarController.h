#pragma once
//this class handles the communication with the TrakSTAR hardware
//functionality such as correct configuration, starting, stopping the scan and
//getting new data is handled here.

#include "NDI/ATC3DG.h"				//TrakSTAR driver
#include <time.h>
#include "Point3.h"
#include <string>

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


	class TrakStarController
	{
	public:
		TrakStarController();

		/// <summary>
		/// Initialie the system. Call this before making a measurement
		/// </summary>
		void Init();
		/// <summary>
		/// Configure the sensor system. Call this after init and before making a measurement.
		/// </summary>
		void Config();
		/// <summary>
		/// Attache the sensors to the system. Call this before making a measurement.
		/// </summary>
		void AttachSensor();

		/// <summary>
		/// Returns the number of sensors in the current config
		/// </summary>
		/// <returns> - number of sensors available</returns>
		int GetNSensors();

		/// <summary>
		/// Get the latest record for a specific sensor
		/// </summary>
		/// <param name="sensorID"> - The ID of the sensor from which the record will be returned</param>
		/// <returns> - The sensor record</returns>
		Point3 GetRecord(int sensorID);

		/// <summary>
		/// Reads a number of records for all sensors and prints it to console.
		/// </summary>
		void ReadSensor();

		/// <summary>
		/// Turn off the referenece.
		/// </summary>
		void StopTransmit();

	private:
		CSystem			ATC3DG;
		CSensor*		pSensor;
		CXmtr*			pXmtr;

		int				errorCode;
		int				i;
		int				sensorID;
		short			id;
		int				records = 100;
		char			output[256];
		int				numberBytes;

		clock_t			goal;
		clock_t			wait = 10;	// 10 ms delay

		void ErrorHandler(int error);

		const std::string GetErrorString(int error);
	};
}
