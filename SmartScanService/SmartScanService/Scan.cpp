#include "Scan.h"
#include "Exceptions.h"
#include <iomanip>
#include <ctime>
#include <cstdio>

using namespace SmartScan;

Scan::Scan(const int id, TrakStarController* pTSCtrl) : mId{ id }, pTSCtrl{ pTSCtrl }
{
    for(int i = 0; i < mConfig.usedSensorIds.size(); i++)
    {
        mInBuff.push_back(std::vector<Point3>());
    }
}

Scan::Scan(const int id, TrakStarController* pTSCtrl, const ScanConfig config) 
    : mId{ id }, pTSCtrl{ pTSCtrl }, mConfig{ config }
{
    for(int i = 0; i < mConfig.usedSensorIds.size(); i++)
    {
        mInBuff.push_back(std::vector<Point3>());
    }
}

Scan::~Scan()
{
	this->Stop(true);
}

void Scan::Run(bool acquisitionOnly)
{
	// Check if reference points have been defined:
	mStopDataAcquisition = mStopFiltering = false;

	// Check wether trak star controller has been initialised 
	if (!pTSCtrl)
	{
		throw ex_trakStar("No track star controller defined", __func__, __FILE__);
	}

	// Check if this scan is already running:
	if (this->mRunning)
	{
		std::cout << "[SCAN] " << "Scan already in progress" << std::endl;
		return;
	}

	// Start the thread:
	std::cout << "[SCAN] " << "Starting scan with id " << mId << std::endl;
	try
	{
		this->pAcquisitionThread = std::make_unique<std::thread>(&Scan::DataAcquisition, this);
	}
	catch (...)
	{
		throw ex_scan("unnable to start thread", __func__, __FILE__);
	}
	// Let it gooooo, let it gooo
	this->pAcquisitionThread->detach();

    // Do not start a filtering thread when dataAcquisition only is the scan config or when calculating reference points.
	if (!mConfig.acquisitionOnly && !acquisitionOnly)
	{
		// Set up Filtering object
		try
		{
			mF.SetReferencePoints(mReferencePoints);
			mF.SetPrecision(mConfig.filteringPrecision, mConfig.filteringPrecision);
			mF.SetFrameSize(frameSize);
		}
		catch (...)
		{
			throw ex_scan("Unable to set Filtering References and Resolution", __func__, __FILE__);
		}

		// Start the filtering thread:
		try
		{
			this->pFilteringThread = std::make_unique<std::thread>(&Scan::DataFiltering, this);
		}
		catch (...)
		{
			throw ex_scan("unnable to start filtering thread", __func__, __FILE__);
		}

		// Let it gooooo, let it gooo
		this->pFilteringThread->detach();
	}
	mRunning = true;
}

void Scan::Stop(bool clearData)
{
	// Check if this scan is already running.
	if (!this->mRunning)
	{
		std::cout << "[SCAN] " << "Scan not running." << std::endl;
		return;
	}
	std::cout << "[SCAN] " << "Stopping the scan. \n";
	mStopDataAcquisition = true;
	mStopFiltering = true;

	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (clearData)
	{
		mInBuff.clear();
		mOutBuff.clear();
		mRefBuff.clear();
	}

	mRunning = false;
}

const bool Scan::IsRunning() const
{
	return mRunning;
}

const double Scan::GetSampleRate() const
{
	return mConfig.sampleRate;
}

void Scan::DataAcquisition()
{
	// Start the data aquisition:
	std::cout << "[SCAN] " << (mInBuff[0].size() > 0? "Resuming" : "Running") <<" data aquisition for " << mConfig.usedSensorIds.size() << " sensors \n";

	// Store time on a variable time which increases the sensor sample time
	double time = 0;
	auto startSampling = std::chrono::steady_clock::now();

	std::chrono::time_point<std::chrono::steady_clock> endSampleTime = startSampling;

	while (!mStopDataAcquisition)
	{
		// Store time and calculate the elapsed time since last sample
		auto startSampleTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedTime = startSampleTime - endSampleTime;
		if (elapsedTime.count() >= 1 / mConfig.sampleRate) 
        {
            time += elapsedTime.count();
            try
            {
                for(int i = 0; i < mConfig.usedSensorIds.size(); i++) 
                {
                    // Make Point3 obj to get the position info of the trackStar device
				    Point3 tmp = pTSCtrl->GetRecord(mConfig.usedSensorIds[i]);	 
					button_obj.UpdateButtonState(tmp.button); 
					tmp.buttonState = button_obj.GetButtonState();
					//std::cout << (int)tmp.buttonState << std::endl;
				    // Add sample time to overal time and store in mInBuff
					tmp.time = time;
					tmp.z = tmp.z * -1;
				    mInBuff[i].push_back(tmp);
				    //std::cout << tmp.time << std::endl;
					if (mRawDataCallback)
					{
						mRawDataCallback(tmp);
					}
                }			
            }
            catch(...)
            {
                throw ex_scan("Failed to get record from sensor", __func__, __FILE__);
            }
      
			// Store current time and calculate duration of the samples
			endSampleTime = std::chrono::steady_clock::now();

			// Make sure we are not slower than the required sample rate:
			elapsedTime = std::chrono::steady_clock::now() - startSampleTime;
			if (elapsedTime.count() > (1 / (mConfig.sampleRate/3)))
			{
				std::cerr << "[SCAN] " << "Sampling is too slow!" << std::endl;
			}
        } 
		
    }
	
	std::cout<< "[SCAN] " << "Data acquisition completed \n";
	mStopDataAcquisition = false;

	std::chrono::duration<double> totalScanTime = std::chrono::steady_clock::now() - startSampling;
	std::cout << "[SCAN] " << mInBuff[0].size() << " samples aquired in the bg during a " << totalScanTime.count() << " seconds scan using a " << mConfig.sampleRate << " Hz sample rate\n";
	std::cout << "[SCAN] " << "Please wait for filtering to complete. \n";
}

// TODO: fix processing the remaining data after acquisition stops when filtering has fallen behind
void Scan::DataFiltering()
{
	// Start the data aquisition:
	std::cout << "[SCAN] " << "Running data filtering \n";

	while (!mStopFiltering)
	{
		auto startTime = std::chrono::steady_clock::now();

		// Check if there is new data available:
		const int inSize = mInBuff[0].size();
		const int outSize = mOutBuff.size();

		if (inSize > (lastFilteredSample) && frameCounter < frameSize)
		{
			// Add the new data to the output buffer:
			// Might remove for efficiency
			try
			{
				mOutBuff.push_back(mInBuff[0].begin()[lastFilteredSample]);
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
				// Done with the frame, filter it:
				// For example, only keep a third (middle):
				while(mRefBuff.size() % (frameSize / NUsedSensors()))
				{
					
				}
				std::vector<Point3>::const_iterator refBufStartIt =  mRefBuff.cbegin() + (lastFilteredSample - frameSize)/NUsedSensors();
				int refBufOffset = (lastFilteredSample- frameSize) / NUsedSensors() + frameSize / NUsedSensors();
				std::vector<Point3>::const_iterator refBufEndIt = mRefBuff.cbegin() + refBufOffset;
				// Make a copy of the ref vector that coresponds to the current frame:
				std::vector<Point3> refCopy(refBufStartIt, refBufEndIt);
				//auto mOutCopy = mOutBuff;
				mF.Filter(mOutBuff, refCopy);
				// When filtering is done, execute the callback:
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
		if (elapsed_seconds.count() > 1/mConfig.sampleRate * frameSize)
		{
			std::cerr << "[SCAN] " << "Filtering "<< elapsed_seconds.count() - (1 /mConfig.sampleRate * frameSize)<< "s slower than real-time! " << std::endl;
		}
	}

	// Handle the leftover stuff:
	if (lastFilteredSample < mInBuff[0].size() - 1)
	{
		// Send the rest to be filtered.
	}

	//std::chrono::duration<double> totalScanTime = std::chrono::steady_clock::now() - scanStartTime;
	//std::cout << "[SCAN] " << "Data filtering completed in " << totalScanTime.count() << " seconds" << std::endl;
	mStopFiltering = false;
}

void Scan::DumpData() const
{
	std::cout << "Time" << "\t" << "X" << "\t" << "Y" << "\t" << "Z" << "\t" << "Rx" << "\t" << "Ry" << "\t" << "Rz" << "\t" << "Quality" << "\t" << "Button" << std::endl;
	for (auto& record : mOutBuff)    // Access by reference to avoid copying
	{
		std::cout << std::setprecision(4) << record.time << "\t" << record.x << "\t" << record.y << "\t" << record.z << "\t" << record.r.x << "\t" << record.r.y << "\t" << record.r.z << "\t"  << record.quality << "\t" << (int)record.button << "\n";
	}
}

void Scan::RegisterNewDataCallback(std::function<void(std::vector<Point3>&)> callback)
{
	mNewDataCallback = callback;
}
void Scan::RegisterRawDataCallback(std::function<void(SmartScan::Point3)> callback)
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

const std::vector<int> SmartScan::Scan::GetUsedSensors() const
{
	return mConfig.usedSensorIds;
}

const int SmartScan::Scan::GetReferenceSensorId()
{
	return mConfig.referenceSensorId;
}

const int SmartScan::Scan::NUsedSensors() const
{
	return mConfig.usedSensorIds.size();
}

const double SmartScan::Scan::GetFilteringPrecision()
{
	return mConfig.filteringPrecision;
}

const bool SmartScan::Scan::IsAcquisitionOnly() const
{
    return mConfig.acquisitionOnly;
}