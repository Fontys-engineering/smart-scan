#include "TrackStar.h"
#include "stdio.h"

//constructor
TrackStar::TrackStar()
{
	TrackStar_init();
}
//init func
void TrackStar::TrackStar_init()
{
	printf("Initializing ATC3DG system...\n");
	errorCode = InitializeBIRDSystem();
	if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);

}
