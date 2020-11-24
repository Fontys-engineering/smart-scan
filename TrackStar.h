#pragma once
#include "ATC3DG.h"
#include "sample.h"
#include "time.h"
class TrackStar
{
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
	
public:
	TrackStar();
	void TrackStar_init();

};




