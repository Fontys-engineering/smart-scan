#include "Scan.h"
#include "Exceptions.h"
#include <iomanip>

using namespace SmartScan;

Scan::Scan(const int id, TrakStarController* pTSCtrl) : mId{ id }, pTSCtrl{ pTSCtrl }, mRefSensorId{-1}
{
}

SmartScan::Scan::Scan(
	const int id, TrakStarController* pTSCtrl, 
	const double sampleRate, 
	const std::vector<int> usedSensors, 
	const int refSensorId) : 
		mId{ id }, 
		pTSCtrl{ pTSCtrl }, 
		mRefSensorId{ refSensorId },
		sampleRate {sampleRate},
		mUsedSensors {usedSensors}
{
}

Scan::~Scan()
{
	this->Stop(true);
}

void Scan::Run(bool acqusitionOnly)
{
	//check if reference points have been defined:
	mStopDataAcquisition = mStopFiltering = false;

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

	if (!acqusitionOnly)
	{
		// Set up Filtering object
		try
		{
			mF.SetReferencePoints(mReferencePoints);
			mF.SetResolution(0, 0);
			mF.SetFrameSize(frameSize);
		}
		catch (...)
		{
			throw ex_scan("Unable to set Filtering References and Resolution", __func__, __FILE__);
		}

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
	}
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
		mRefBuff.clear();
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
						mInBuff.push_back(pTSCtrl->GetRecord(i));
					}
					else
					{
						for (int id = 0; id<mUsedSensors.size();id++)
						{
							if (i == id)
							{
								mInBuff.push_back(pTSCtrl->GetRecord(i));
							}
						}
					}

					if (i == mRefSensorId)
					{
						mRefBuff.push_back(pTSCtrl->GetRecord(i));
					}
				}
				catch (...)
				{
					throw ex_scan("Failed to get record from sensor", __func__, __FILE__);
				}
			}
			//raw data callback
			if (mRawDataCallback)
				mRawDataCallback(mInBuff);

			//make sure we are not slower than the required sample rate:
			elapsed_seconds = std::chrono::steady_clock::now() - startTime;
			if (elapsed_seconds.count() > (1 / (sampleRate/3)))
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

	std::cout << "[SCAN] " << "Please wait for filtering to complete. \n";
}

void Scan::DataFiltering()
{
	//start the data aquisition:
	std::cout << "[SCAN] " << "Running data filtering \n";

	while (!mStopFiltering || (mStopFiltering && lastFilteredSample < mInBuff.size()))
	{
		auto startTime = std::chrono::steady_clock::now();
		

		//check if there is new data available:
		const int inSize = mInBuff.size();
		const int outSize = mOutBuff.size();

		if (inSize > (lastFilteredSample) && frameCounter < frameSize)
		{
			//add the new data to the output buffer:
			try
			{
				mOutBuff.push_back(mInBuff.begin()[lastFilteredSample]);
				++frameCounter;
				++lastFilteredSample;

			}
			catch (...)
			{
				throw ex_scan("Could not get sample from input buffer", __func__, __FILE__);
			}
		}
		else if (frameCounter >= frameSize){
			try
			{
				//done with the frame, filter it:
				//for example, only keep a third (middle):
				//auto refCopy = mRefBuff;
				//auto mOutCopy = mOutBuff;
				mF.Filter(mOutBuff);
				mRefBuff.clear();
				//when filtering is done, execute the callback:
				if(mNewDataCallback)
					mNewDataCallback(mOutBuff);

				frameCounter = 0;
			}
			catch (...)
			{
				throw "nasty";
			}
		}

		std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - startTime;
		if (elapsed_seconds.count() > 1/sampleRate * frameSize)
		{
			std::cerr << "[SCAN] " << "Filtering "<< elapsed_seconds.count() - (1 / sampleRate * frameSize)<< "s slower than real-time! " << std::endl;
		}
	}
	std::chrono::duration<double> totalScanTime = std::chrono::steady_clock::now() - scanStartTime;
	std::cout << "[SCAN] " << "Data filtering completed in " << totalScanTime.count() << " seconds" << std::endl;
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
void Scan::RegisterRawDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mRawDataCallback = callback;
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
const std::vector<int> SmartScan::Scan::GetUsedSensors() const
{
	return this->mUsedSensors;
}
void Scan::SetUsedSensors()
{
	mUsedSensors.clear();
}

void SmartScan::Scan::SetReferenceSensorId(const int sensorId)
{
	mRefSensorId = sensorId;
}

const int SmartScan::Scan::GetReferenceSensorId()
{
	return mRefSensorId;
}

const int SmartScan::Scan::NUsedSensors() const
{
	return mUsedSensors.size();
}
