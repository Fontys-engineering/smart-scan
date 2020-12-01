#include "Scan.h"

using namespace SmartScan;

Scan::Scan(const int id) : mId{id}
{
	
}

void Scan::Start()
{
	//check if reference points have been defined:

	//check wether trak star controller has been initialised 

	//start the data aquisition:
	std::cout << "starting data aquisition \n";
	while (!mStopDataAcquisition)
	{

	}
	
	std::cout << "Data acquisition completed \n";
	mStopDataAcquisition = false;
}

void Scan::Stop(bool clearData)
{
	std::cout << "Stopping the scan \n";
	mStopDataAcquisition = true;
}