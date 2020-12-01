#include "Scan.h"

using namespace SmartScan;

Scan::Scan(const int id, TrakStarController* pTSCtrl) : mId{ id }
{
	this->pTSCtrl = pTSCtrl;
}

void Scan::Run()
{
	//check if reference points have been defined:

	//check wether trak star controller has been initialised 
	if (!pTSCtrl)
	{
		throw "No TrakSTAR Controller defined";
	}
	
	//start the thread:
	this->acquisitionThread = std::make_unique<std::thread>(&Scan::DataAcquisition, this);
	std::cout << "stuff \n";
	std::cout << "stuff \n";
	std::cout << "stuff \n";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	Stop();
}

void Scan::Stop(bool clearData)
{

	std::cout << "Stopping the scan \n";
	mStopDataAcquisition = true;

	this->acquisitionThread.get()->join();
}

void Scan::DataAcquisition()
{
	//start the data aquisition:
	std::cout << "starting data aquisition \n";
	unsigned int samples = 0;

	while (!mStopDataAcquisition)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		samples++;
	}

	std::cout << samples<<" samples aquired in the bg \n";

	std::cout << "Data acquisition completed \n";
	mStopDataAcquisition = false;
}