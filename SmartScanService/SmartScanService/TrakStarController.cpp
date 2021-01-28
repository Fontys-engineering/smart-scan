#include "TrakStarController.h"

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "Exceptions.h"

using namespace SmartScan;

//constructor
TrakStarController::TrakStarController(bool mock): mMock {mock}
{
	if (mock)
	{
		/* initialize random seed: */
		srand(time(NULL));
	}
}
//init func
void TrakStarController::Init()
{
	//when in Mock mode do nothing:
	if (mMock)
	{
		return;
	}
	//printf("Initializing ATC3DG system...\n");
	errorCode = InitializeBIRDSystem();
	if (errorCode != BIRD_ERROR_SUCCESS)
	{
		//ErrorHandler(errorCode);
		throw ex_trakStar(GetErrorString(errorCode), __func__, __FILE__);
	}
}

void TrakStarController::Config()
{
	//when in mock mode do nothing:
	if (mMock)
	{
		return;
	}
	//system
	errorCode = GetBIRDSystemConfiguration(&ATC3DG.m_config);
	if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);

	//sensor
	pSensor = new CSensor[ATC3DG.m_config.numberSensors];
	for (i = 0; i < ATC3DG.m_config.numberSensors; i++)
	{
		errorCode = GetSensorConfiguration(i, &(pSensor + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
	}

	//transmitter
	pXmtr = new CXmtr[ATC3DG.m_config.numberTransmitters];
	for (i = 0; i < ATC3DG.m_config.numberTransmitters; i++)
	{
		errorCode = GetTransmitterConfiguration(i, &(pXmtr + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
	}

	SET_SYSTEM_PARAMETER(SELECT_TRANSMITTER, 0);
	SET_SYSTEM_PARAMETER(POWER_LINE_FREQUENCY, 50.0);
	SET_SYSTEM_PARAMETER(MEASUREMENT_RATE, 50);
	SET_SYSTEM_PARAMETER(MAXIMUM_RANGE, 72.0);
	SET_SYSTEM_PARAMETER(METRIC, true);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//
	// The SYSTEM_CONFIGURATION structure filled out by the initialization proc
	// contains the following:
	printf("Number Boards          = %d\n", ATC3DG.m_config.numberBoards);
	printf("Number Sensors         = %d\n", ATC3DG.m_config.numberSensors);
	printf("Number Transmitters    = %d\n\n", ATC3DG.m_config.numberTransmitters);

	printf("System AGC mode	       = %d\n", ATC3DG.m_config.agcMode);
	printf("Maximum Range          = %6.2f\n", ATC3DG.m_config.maximumRange);
	printf("Measurement Rate       = %10.6f\n", ATC3DG.m_config.measurementRate);
	printf("Metric Mode            = %d\n", ATC3DG.m_config.metric);
	printf("Line Frequency         = %6.2f\n", ATC3DG.m_config.powerLineFrequency);
	printf("Transmitter ID Running = %d\n", ATC3DG.m_config.transmitterIDRunning);

}

void TrakStarController::AttachSensor()
{
	//when in mock mode do nothing:
	if (mMock)
	{
		return;
	}
	for (id = 0; id < ATC3DG.m_config.numberTransmitters; id++)
	{
		if ((pXmtr + id)->m_config.attached)
		{
			// Transmitter selection is a system function.
			// Using the SELECT_TRANSMITTER parameter we send the id of the
			// transmitter that we want to run with the SetSystemParameter() call
			errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
			if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
			break;
		}
	}
}


int TrakStarController::GetNSensors()
{
	//when in mock mode use 4 sensors
	if (mMock)
	{
		return 4;
	}

	if (&ATC3DG)
	{
		return ATC3DG.m_config.numberSensors;
	}
	else
	{
		throw "Sensor config unnavailable.";
	}
}

Point3 TrakStarController::GetRecord(int sensorID)
{
	//when in mock mode, return a random value on a sphere:
	if (mMock)
	{
		//return GetMockRecord();
		return GetMockRecordFromFile();
	}
	++sensorID;
	if (sensorID > ATC3DG.m_config.numberSensors || sensorID <= 0)
	{
		throw ex_smartScan( "Sensor ID out if range", __func__, __FILE__);
	}

	DOUBLE_POSITION_ANGLES_RECORD record, * pRecord = &record;

	// sensor attached so get record
	try
	{
		errorCode = GetAsynchronousRecord(sensorID, pRecord, sizeof(record));
	}
	catch(...)
	{
		if (errorCode != BIRD_ERROR_SUCCESS) 
		{ 
			ErrorHandler(errorCode); 
		}
	}
	

	// get the status of the last data record
	// only report the data if everything is okay
	unsigned int status = GetSensorStatus(sensorID);

	if (status == VALID_STATUS)
	{
		return Point3(record.x, record.y, record.z, record.a, record.e, record.r);
	}
	else
	{
		throw ex_trakStar("No valid sensor record found", __func__, __FILE__);
	}

	return Point3();
}

void TrakStarController::ReadSensor()
{
	//when in mock mode do nothing:
	if (mMock)
	{
		return;
	}
	DOUBLE_POSITION_ANGLES_RECORD record, * pRecord = &record;

	// Set up time delay for first loop
	// It only makes sense to request a data record from the sensor once per
	// measurement cycle. Therefore we set up a 10ms loop and request a record
	// only after at least 10ms have elapsed.
	//
	goal = wait + clock();

	// collect as many records as specified in the command line
	for (i = 0; i < records; i++)
	{
		// delay 10ms between collecting data
		// wait till time delay expires
		while (goal > clock());
		// set up time delay for next loop
		goal = wait + clock();

		// scan the sensors and request a record
		for (sensorID = 0; sensorID < ATC3DG.m_config.numberSensors; sensorID++)
		{
			// sensor attached so get record
			errorCode = GetAsynchronousRecord(sensorID, pRecord, sizeof(record));
			if (errorCode != BIRD_ERROR_SUCCESS) { ErrorHandler(errorCode); }

			// get the status of the last data record
			// only report the data if everything is okay
			unsigned int status = GetSensorStatus(sensorID);

			if (status == VALID_STATUS)
			{
				// send output to console
				sprintf_s(output, "[%d] %8.3f %8.3f %8.3f: %8.2f %8.2f %8.2f\n",
					sensorID,
					record.x,
					record.y,
					record.z,
					record.a,
					record.e,
					record.r
				);
				numberBytes = strlen(output);
				printf("%s", output);
			}
		}
	}


}

void TrakStarController::StopTransmit()
{
	//when in mock mode do nothing:
	if (mMock)
	{
		return;
	}
	id = -1;
	errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
	if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);

	delete[] pSensor;
	delete[] pXmtr;
}

Point3 TrakStarController::GetMockRecord()
{
	double radius = 100;
	int randomMax = 10;
	double randomMaxRadius = 200;

	//return Point3();
	
	if (mPrevMockRecord.x == 0 && mPrevMockRecord.y == 0 && mPrevMockRecord.z == 0)
	{
		//first mock value:
		mPrevMockRecord = Point3(radius, 0, 0);
		return mPrevMockRecord;
	}
	else {
		//add some random noise to the previous mockRecord;
		mPrevMockRecord = Point3(mPrevMockRecord.x + ((rand() % randomMax) - randomMax / 2), mPrevMockRecord.y + ((rand() % randomMax) - randomMax / 2), mPrevMockRecord.z + ((rand() % randomMax) - randomMax / 2));
		return mPrevMockRecord;
	}
}

Point3 TrakStarController::GetMockRecordFromFile()
{
	//open the file if not already open:
	if (!mockDataFile.is_open())
	{
		mockDataFile.open(mockDataFilePath, std::ifstream::in);
	}

	//get the next line from the file
	Point3 newPoint;
	char line[512];
	if (!mockDataFile.getline(line,512))
	{
		//no more lines to read
		//close the file
		mockDataFile.close();
		//try again:
		return GetMockRecordFromFile();
	}
	//parse the coorindates from the line:
	for (int c = 5; c >= 0; c--)
	{
		//find the last comma
		char* found = line;
		if (c)
		{
			found = strrchr(line, ',');
			++found;
		}

		//parse the number after that comma:
		float f = std::atof(found);

		//delete the number from the line
		if (c) {
			*(--found) = '\0';
		}

		//add it to the point:
		switch (c)
		{
		case 0:
			newPoint.x = f;
			break;
		case 1:
			newPoint.y = f;
			break;
		case 2:
			newPoint.z = f;
			break;
		case 3:
			newPoint.r.x = f;
			break;
		case 4:
			newPoint.r.y = f;
			break;
		case 5:
			newPoint.r.z = f;
			break;
		}
	}

	return newPoint;
}


void TrakStarController::ErrorHandler(int error)
{
	char			buffer[1024];
	char* pBuffer = &buffer[0];
	int				numberBytes;

	while (error != BIRD_ERROR_SUCCESS)
	{
		error = GetErrorText(error, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);
		numberBytes = strlen(buffer);

		printf("%s", buffer);
		//throw ex_trakStar(buffer, __func__, __FILE__);
	}
}

const std::string TrakStarController::GetErrorString(int error)
{
	char			buffer[1024];
	std::string errorString;

	if (error != BIRD_ERROR_SUCCESS)
	{
		error = GetErrorText(error, buffer, sizeof(buffer), SIMPLE_MESSAGE);
		errorString =  buffer;
		return errorString;
	}
}
