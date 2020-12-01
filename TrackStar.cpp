#include "TrackStar.h"
#include "stdio.h"
#include "stdafx.h"

//constructor
TrackStar::TrackStar()
{
	TrackStar_init();
	TrackStar_config();
	TrackStar_attachsensor();
	TrackStar_readsensor();
	TrackStar_stoptransmit();
}
//init func
void TrackStar::TrackStar_init()
{
	printf("Initializing ATC3DG system...\n");
	errorCode = InitializeBIRDSystem();
	if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);

}

void TrackStar::TrackStar_config()
{
	//system
	errorCode = GetBIRDSystemConfiguration(&ATC3DG.m_config);
	if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	//sensor
	pSensor = new CSensor[ATC3DG.m_config.numberSensors];
	for (i = 0; i < ATC3DG.m_config.numberSensors; i++)
	{
		errorCode = GetSensorConfiguration(i, &(pSensor + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}
	//transmitter
	pXmtr = new CXmtr[ATC3DG.m_config.numberTransmitters];
	for (i = 0; i < ATC3DG.m_config.numberTransmitters; i++)
	{
		errorCode = GetTransmitterConfiguration(i, &(pXmtr + i)->m_config);
		if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);
	}

}

void TrackStar::TrackStar_attachsensor()
{
	for (id = 0; id < ATC3DG.m_config.numberTransmitters; id++)
	{
		if ((pXmtr + id)->m_config.attached)
		{
			// Transmitter selection is a system function.
			// Using the SELECT_TRANSMITTER parameter we send the id of the
			// transmitter that we want to run with the SetSystemParameter() call
			errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
			if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);
			break;
		}
	}


}

void TrackStar::TrackStar_readsensor()
{
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
			if (errorCode != BIRD_ERROR_SUCCESS) { errorHandler(errorCode); }

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

void TrackStar::TrackStar_stoptransmit()
{
	id = -1;
	errorCode = SetSystemParameter(SELECT_TRANSMITTER, &id, sizeof(id));
	if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);

	delete[] pSensor;
	delete[] pXmtr;

}



void errorHandler(int error)
{
	char			buffer[1024];
	char* pBuffer = &buffer[0];
	int				numberBytes;

	while (error != BIRD_ERROR_SUCCESS)
	{
		error = GetErrorText(error, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);
		numberBytes = strlen(buffer);
		buffer[numberBytes] = '\n';		// append a newline to buffer
		printf("%s", buffer);
	}
	exit(0);
}
