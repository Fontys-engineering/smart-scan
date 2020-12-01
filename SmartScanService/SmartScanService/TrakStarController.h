#pragma once
//this class handles the communication with the TrakSTAR hardware
//functionality such as correct configuration, starting, stopping the scan and
//getting new data is handled here.

#include "NDI/ATC3DG.h"				//TrakSTAR driver
#include <time.h>

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


		void Init();

	private:
		CSystem			ATC3DG;
		CSensor* pSensor;
		CXmtr* pXmtr;
		int				errorCode;
		int				i;
		int				sensorID;
		short			id;
		int				records = 100;
		char			output[256];
		int				numberBytes;
		clock_t			goal;
		clock_t			wait = 10;	// 10 ms delay
	};
}
