#pragma once
#include "ATC3DG.h"
#include "sample.h"
#include "time.h"

class TrackStar
{
private:

	
public:
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

	TrackStar();
	void TrackStar_init();
	void TrackStar_config();
	void TrackStar_attachsensor();
	void TrackStar_readsensor();
	void TrackStar_stoptransmit();

};

void errorHandler(int error);








