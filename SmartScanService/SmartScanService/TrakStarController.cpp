#include "TrakStarController.h"

using namespace SmartScan;

//constructor
TrakStarController::TrakStarController()
{
	
}
//init func
void TrakStarController::Init()
{
	printf("Initializing ATC3DG system...\n");
	errorCode = InitializeBIRDSystem();
	if (errorCode != BIRD_ERROR_SUCCESS) errorHandler(errorCode);

}