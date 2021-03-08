#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Exceptions.h"
#include "TrakStarController.h"

using namespace SmartScan;

TrakStarController::TrakStarController(bool mock) : mMock{ mock }
{
	if (mock)
	{
		// Initialize random seed.
		srand(time(NULL));
	}
}

void TrakStarController::Init()
{
	if (mMock)
	{
		return;
	}

	errorCode = InitializeBIRDSystem();
	if (errorCode != BIRD_ERROR_SUCCESS)
	{
		//ErrorHandler(errorCode);
		throw ex_trakStar(GetErrorString(errorCode), __func__, __FILE__);
	}
}

void TrakStarController::Config()
{
	if (mMock)
	{
		return;
	}

	errorCode = GetBIRDSystemConfiguration(&ATC3DG.m_config);
	if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);

	pSensor = new CSensor[ATC3DG.m_config.numberSensors];
	for (i = 0; i < ATC3DG.m_config.numberSensors; i++)
	{
		errorCode = GetSensorConfiguration(i, &(pSensor + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
	}

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

	errorCode = GetBIRDSystemConfiguration(&ATC3DG.m_config);
	if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);

	pSensor = new CSensor[ATC3DG.m_config.numberSensors];
	for (i = 0; i < ATC3DG.m_config.numberSensors; i++)
	{
		errorCode = GetSensorConfiguration(i, &(pSensor + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
	}

	pXmtr = new CXmtr[ATC3DG.m_config.numberTransmitters];
	for (i = 0; i < ATC3DG.m_config.numberTransmitters; i++)
	{
		errorCode = GetTransmitterConfiguration(i, &(pXmtr + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
	}

	// The SYSTEM_CONFIGURATION structure filled out by the initialization proc
	// contains the following:
	std::cout << "_____________________________________________________Configuration_____________________________________________________" << std::endl;
    std::cout << "\tNumber Boards \t\t\t = " << ATC3DG.m_config.numberBoards << std::endl;
	std::cout << "\tNumber Boards \t\t\t = " << ATC3DG.m_config.numberBoards << std::endl;
	std::cout << "\tNumber Sensors \t\t\t = " << ATC3DG.m_config.numberSensors << std::endl;
	std::cout << "\tNumber Transmitters \t\t = " << ATC3DG.m_config.numberTransmitters << std::endl;
    std::cout << std::endl;
	std::cout << "\tSystem AGC mode	\t\t = " << ATC3DG.m_config.agcMode << std::endl;
	std::cout << "\tMaximum Range \t\t\t = " << ATC3DG.m_config.maximumRange << std::endl;
	std::cout << "\tMeasurement Rate \t\t = " << ATC3DG.m_config.measurementRate << std::endl;
	std::cout << "\tMetric Mode \t\t\t = " << ATC3DG.m_config.metric << std::endl;
	std::cout << "\tLine Frequency \t\t\t = " << ATC3DG.m_config.powerLineFrequency << std::endl;
	std::cout << "\tTransmitter ID Running \t\t = " << ATC3DG.m_config.transmitterIDRunning << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < ATC3DG.m_config.numberSensors; i++)
    {
        std::cout << "Port " << pSensor[i].m_config.channelNumber << "has ";
        if (pSensor[i].m_config.attached) std::cout << "sensor attached with serial number " << pSensor[i].m_config.serialNumber;
        else std::cout << "no sensor attached";
        std::cout << std::endl;
    }

	std::cout << "_______________________________________________________________________________________________________________________" << std::endl;
}

void TrakStarController::AttachSensor()
{
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
			// transmitter that we want to run with the SetSystemParameter() call.
			errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
			if (errorCode != BIRD_ERROR_SUCCESS) ErrorHandler(errorCode);
			break;
		}
	}
}

int TrakStarController::GetNSensors()
{
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
	// When in mock mode, return a random value on a sphere.
	if (mMock)
	{
		//return GetMockRecord();
		return GetMockRecordFromFile(sensorID);
	}
	++sensorID;
	if (sensorID > ATC3DG.m_config.numberSensors || sensorID <= 0)
	{
		throw ex_smartScan("Sensor ID out if range", __func__, __FILE__);
	}

	DOUBLE_POSITION_ANGLES_RECORD record, * pRecord = &record;

	// Sensor attached so get record.
	try
	{
		errorCode = GetAsynchronousRecord(sensorID, pRecord, sizeof(record));
	}
	catch (...)
	{
		if (errorCode != BIRD_ERROR_SUCCESS)
		{
			ErrorHandler(errorCode);
		}
	}


	// Get the status of the last data record.
	// Only report the data if everything is okay.
	unsigned int status = GetSensorStatus(sensorID);

	if (status == VALID_STATUS)
	{
		return Point3(record.x, record.y, record.z, record.r, record.e, record.a);
	}
	else
	{
		throw ex_trakStar("No valid sensor record found", __func__, __FILE__);
	}

	return Point3();
}

//This function is not used
void TrakStarController::ReadSensor()
{
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
		// First mock value.
		mPrevMockRecord = Point3(radius, 0, 0);
		return mPrevMockRecord;
	}
	else {
		// Add some random noise to the previous mockRecord.
		mPrevMockRecord = Point3(mPrevMockRecord.x + ((rand() % randomMax) - randomMax / 2), mPrevMockRecord.y + ((rand() % randomMax) - randomMax / 2), mPrevMockRecord.z + ((rand() % randomMax) - randomMax / 2));
		return mPrevMockRecord;
	}
}

Point3 TrakStarController::GetMockRecordFromFile(int sensorId)
{
	// Open the file if not already open.
	if (!s0MockDataFile.is_open())
	{
		s0MockDataFile.open(s0MockDataFilePath, std::ifstream::in);
	}
	if (!s1MockDataFile.is_open())
	{
		s1MockDataFile.open(s1MockDataFilePath, std::ifstream::in);
	}
	if (!s2MockDataFile.is_open())
	{
		s2MockDataFile.open(s2MockDataFilePath, std::ifstream::in);
	}

	// Get the next line from the file.
	Point3 newPoint;
	char line[512];
	if (sensorId == 0 && !s0MockDataFile.getline(line, 512))
	{
		// No more lines to read.
		// Close the file.
		s0MockDataFile.close();
		// Try again.
		return GetMockRecordFromFile(sensorId);
	}
	else if (sensorId == 1 && !s1MockDataFile.getline(line, 512))
	{
		// No more lines to read.
		// Close the file.
		s1MockDataFile.close();
		// Try again.
		return GetMockRecordFromFile(sensorId);
	}
	else if (sensorId == 2 && !s2MockDataFile.getline(line, 512))
	{
		//no more lines to read
		//close the file
		s2MockDataFile.close();
		//try again:
		return GetMockRecordFromFile(sensorId);
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
		errorString = buffer;
		return errorString;
	}
}