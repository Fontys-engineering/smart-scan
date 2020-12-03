#include "Scan.h"
#include "Exceptions.h"
#include <iomanip>

using namespace SmartScan;

Scan::Scan(const int id, TrakStarController* pTSCtrl) : mId{ id }
{
	this->pTSCtrl = pTSCtrl;
}

//Scan::~Scan()
//{
//	//this->Stop();
//	//this->acquisitionThread->join();
//}

void Scan::Run()
{
	//check if reference points have been defined:

	//check wether trak star controller has been initialised 
	if (!pTSCtrl)
	{
		//throw ex_trakStar("No track star controller defined", "Scan::Run", __FILE__);
	}

	//start the thread:
	std::cout << "Starting scan with id " << mId << std::endl;
	try
	{
		this->acquisitionThread = std::make_unique<std::thread>(&Scan::DataAcquisition, this);
	}
	catch (...)
	{
		throw ex_scan("unnable to start thread", "Scan::Run", __FILE__);
	}
	//let it gooooo, let it gooo
	this->acquisitionThread->detach();
}

void Scan::Stop(bool clearData)
{

	std::cout << "Stopping the scan \n";
	mStopDataAcquisition = true;
}

void Scan::DataAcquisition()
{
	//start the data aquisition:
	std::cout << "Running data aquisition \n";

	while (!mStopDataAcquisition)
	{
		auto startTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = startTime - lastSampleTime;
		if (elapsed_seconds.count() >= 1 / sampleRate)
		{
			//sample
			std::chrono::duration<double> totalTime = scanStartTime - startTime;
			mInBuff.emplace_back(Point3(totalTime.count(), 0, 0));

			//make sure we are not slower than the required sample rate:
			elapsed_seconds = std::chrono::steady_clock::now() - startTime;
			if (elapsed_seconds.count() > (1 / sampleRate))
			{
				std::cerr << "Sampling is too slow!" << std::endl;
			}
			//save current time
			lastSampleTime = std::chrono::steady_clock::now();
		}

	}

	std::cout << "Data acquisition completed \n";
	mStopDataAcquisition = false;

	std::chrono::duration<double> totalScanTime = std::chrono::steady_clock::now() - scanStartTime;
	std::cout << mInBuff.size() << " samples aquired in the bg during a " << totalScanTime.count() << " seconds scan using a "<< sampleRate <<" Hz sample rate\n";
}

void Scan::DumpData()
{
	for (auto& record : mInBuff) // access by reference to avoid copying
	{
		std::cout<< std::setprecision(10) << record.x << "\t" << record.y << "\t" << record.z << "\t" << record.r.x << "\t" << record.r.y << "\t" << record.r.z << "\n";
	}
}