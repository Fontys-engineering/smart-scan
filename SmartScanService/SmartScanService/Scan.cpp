#include "Scan.h"
#include "Exceptions.h"
#include <iomanip>

using namespace SmartScan;

Scan::Scan(const int id, TrakStarController* pTSCtrl) : mId{ id }
{
	this->pTSCtrl = pTSCtrl;
}

Scan::~Scan()
{
	this->Stop(true);
}

void Scan::Run()
{
	//check if reference points have been defined:

	//check wether trak star controller has been initialised 
	if (!pTSCtrl)
	{
		throw ex_trakStar("No track star controller defined", __func__, __FILE__);
	}

	//check if this scan is already running:
	if (this->mRunning)
	{
		std::cout << "[SCAN] " << "Scan already in progress" << std::endl;
		return;
	}

	//start the thread:
	std::cout << "[SCAN] " << "Starting scan with id " << mId << std::endl;
	try
	{
		this->pAcquisitionThread = std::make_unique<std::thread>(&Scan::DataAcquisition, this);
	}
	catch (...)
	{
		throw ex_scan("unnable to start thread", __func__, __FILE__);
	}
	//let it gooooo, let it gooo
	this->pAcquisitionThread->detach();


	//start the filtering thread:
	try
	{
		this->pFilteringThread = std::make_unique<std::thread>(&Scan::DataFiltering, this);
	}
	catch (...)
	{
		throw ex_scan("unnable to start filtering thread", __func__, __FILE__);
	}

	//let it gooooo, let it gooo
	this->pFilteringThread->detach();

	mRunning = true;
}

void Scan::Stop(bool clearData)
{
	//check if this scan is already running:
	if (!this->mRunning)
	{
		std::cout << "[SCAN] " << "Scan not running." << std::endl;
		return;
	}
	std::cout << "[SCAN] " << "Stopping the scan. \n";
	mStopDataAcquisition = true;
	mStopFiltering = true;

	//wait a bit for the other threads to finish:
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	if (clearData)
	{
		mInBuff.clear();
		mOutBuff.clear();
	}

	mRunning = false;
}

const bool Scan::isRunning() const
{
	return mRunning;
}

void Scan::SetSampleRate(const double sampleRate)
{
	this->sampleRate = sampleRate;
}
const double Scan::GetSampleRate() const
{
	return this->sampleRate;
}


void Scan::DataAcquisition()
{
	//start the data aquisition:
	std::cout << "[SCAN] " << (mInBuff.size() > 0? "Resuming" : "Running") <<" data aquisition for " << ((mUsedSensors.size() > 0) ? mUsedSensors.size() : pTSCtrl->GetNSensors()) << " sensors \n";
	Point3 newSample;

	while (!mStopDataAcquisition)
	{
		auto startTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = startTime - lastSampleTime;
		if (elapsed_seconds.count() >= 1 / sampleRate)
		{
			//sample:
			//std::chrono::duration<double> totalTime = scanStartTime - startTime;

			for (int i = 0; i < pTSCtrl->GetNSensors(); i++)
			{
				try
				{
					//only sample the sensors we are interested in:
					if (mUsedSensors.size() == 0)
					{
						newSample = pTSCtrl->GetRecord(i);
						mInBuff.push_back(newSample);
					}
					else
					{
						for (auto id : mUsedSensors)
						{
							if (i == id)
							{
								newSample = pTSCtrl->GetRecord(i);
								mInBuff.push_back(newSample);
							}
						}
					}
				}
				catch (...)
				{
					throw ex_scan("Failed to get record from sensor", __func__, __FILE__);
				}
			}
			//make sure we are not slower than the required sample rate:
			elapsed_seconds = std::chrono::steady_clock::now() - startTime;
			if (elapsed_seconds.count() > (1 / sampleRate))
			{
				std::cerr << "[SCAN] " << "Sampling is too slow!" << std::endl;
			}
			//save current time
			lastSampleTime = std::chrono::steady_clock::now();
		}
	}

	std::cout<< "[SCAN] " << "Data acquisition completed \n";
	mStopDataAcquisition = false;

	std::chrono::duration<double> totalScanTime = std::chrono::steady_clock::now() - scanStartTime;
	std::cout << "[SCAN] " << mInBuff.size() << " samples aquired in the bg during a " << totalScanTime.count() << " seconds scan using a " << sampleRate << " Hz sample rate\n";
}

void Scan::DataFiltering()
{
	//start the data aquisition:
	std::cout << "[SCAN] " << "Running data filtering \n";

	while (!mStopFiltering)
	{
		//check if there is new data available:
		const int inSize = mInBuff.size();
		const int outSize = mOutBuff.size();

		if (mInBuff.size() > mOutBuff.size()) {
			//for now just add it to the out buff:
			try
			{
				if (outSize < inSize && inSize > 0)
				{
					mOutBuff.push_back(mInBuff[inSize-1]);
					//new data added. refresh the UI if callback available:
					if (mNewDataCallback)
					{
						mNewDataCallback(mOutBuff);
					}
				}
			}
			catch (...)
			{
				throw ex_scan("Can't add record to output buffer", __func__, __FILE__);
			}
		}
	}

	std::cout << "[SCAN] " << "Data filtering completed \n";
	mStopFiltering = false;
}

void Scan::DumpData() const
{
	for (auto& record : mInBuff) // access by reference to avoid copying
	{
		std::cout << std::setprecision(16) << record.x << "\t" << record.y << "\t" << record.z << "\t" << record.r.x << "\t" << record.r.y << "\t" << record.r.z << "\n";
	}
}

void Scan::RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mNewDataCallback = callback;
}

void Scan::AddReference(const ReferencePoint ref)
{
	mReferencePoints.push_back(ref);
}

const std::vector<ReferencePoint>& Scan::GetReferences() const
{
	return mReferencePoints;
}

void Scan::ResetReferences()
{
	mReferencePoints.clear();
}

void Scan::SetUsedSensors(const std::vector<int> usedSensors)
{
	for (auto id : usedSensors)
	{
		if (id >= pTSCtrl->GetNSensors())
		{
			throw ex_scan("Cannot set used sensors list. Sensor ID out of range.", __func__, __FILE__);
		}
	}
	mUsedSensors = usedSensors;
}  
void Scan::SetUsedSensors()
{
	mUsedSensors.clear();
}