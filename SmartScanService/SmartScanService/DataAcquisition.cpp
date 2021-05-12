#include <iomanip>
#include <ctime>
#include <cstdio>

#include "DataAcquisition.h"
#include "Exceptions.h"

using namespace SmartScan;

DataAcq::DataAcq(bool mock, const DataAcqConfig config) : mMock { mock }, mConfig { config }, mTSCtrl { TrakStarController(mock) }
{

}

DataAcq::~DataAcq()
{
	this->Stop(true);
}

void DataAcq::Init()
{
	mTSCtrl.Init();
	mTSCtrl.Config();
	mTSCtrl.AttachTransmitter();

	mIndexBuff = mTSCtrl.GetAttachedSensors();

	for (int i = 0; i < mIndexBuff.size(); i++)
	{
		mRawBuff.push_back(std::vector<Point3>());
		mSerialBuff.push_back(mTSCtrl.GetSerialFromSensorId());
	}
}


void DataAcq::Start()
{
	// Check wether trak star controller has been initialised 
	if (mRawBuff.size())
	{
		throw ex_trakStar("Data acquisition is not initialized", __func__, __FILE__);
	}

	// Check if this scan is already running:
	if (this->mRunning)
	{
		std::cout << "[SCAN] " << "DataAcquisition already in progress" << std::endl;
		return;
	}

	try
	{
		this->pAcquisitionThread = std::make_unique<std::thread>(&DataAcq::DataAcquisition, this);
	}
	catch (...)
	{
		throw ex_scan("unnable to start thread", __func__, __FILE__);
	}
	// Let it gooooo, let it gooo
	this->pAcquisitionThread->detach();

	mRunning = true;
}

void DataAcq::Stop(bool clearData)
{
	// Check if already running.
	if (!this->mRunning)
	{
		std::cout << "[SCAN] " << "DataAcquisition is already stopped." << std::endl;
		return;
	}

	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (clearData)
	{
		for (int i = 0; i < mRawBuff.size(); i++)
		{
			mRawBuff.at(i).clear();
		}
	}

	mRunning = false;
}

const bool DataAcq::IsRunning() const
{
	return mRunning;
}

const double DataAcq::GetMeasurementRate() const
{
	return mConfig.measurementRate;
}

void DataAcq::DataAcquisition()
{
	// Start the data aquisition:
	std::cout << "[SCAN] " << (mRawBuff[0].size() > 0? "Resuming" : "Running") <<" data aquisition for " << mIndexBuff.size() << " sensors \n";

	// Store time on a variable time which increases the sensor sample time
	double time = 0;
	auto startSampling = std::chrono::steady_clock::now();

	std::chrono::time_point<std::chrono::steady_clock> endSampleTime = startSampling;

	while (!mRunning)
	{
		// Store time and calculate the elapsed time since last sample
		auto startSampleTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedTime = startSampleTime - endSampleTime;
		if (elapsedTime.count() >= 1 / mConfig.measurementRate) 
        {
            time += elapsedTime.count();
            try
            {
                for(int i = 0; i < mIndexBuff.size(); i++) 
                {
                    // Make Point3 obj to get the position info of the trackStar device
				    Point3 tmp = mTSCtrl.GetRecord(mIndexBuff[i]); 
					button_obj.UpdateButtonState(tmp.button); 
					tmp.buttonState = button_obj.GetButtonState();
				    // Add sample time to overal time and store in mInBuff
					tmp.time = time;
					tmp.z = tmp.z * -1;
				    mRawBuff[i].push_back(tmp);
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
			if (elapsedTime.count() > (1 / (mConfig.measurementRate/3)))
			{
				std::cerr << "[SCAN] " << "Sampling is too slow!" << std::endl;
			}
        } 
		
    }
}

void DataAcq::DumpData() const
{
	std::cout << "Time" << "\t" << "X" << "\t" << "Y" << "\t" << "Z" << "\t" << "Rx" << "\t" << "Ry" << "\t" << "Rz" << "\t" << "Quality" << "\t" << "Button" << std::endl;
	for (auto& record : mRawBuff[0])    // Access by reference to avoid copying
	{
		std::cout << std::setprecision(4) << record.time << "\t" << record.x << "\t" << record.y << "\t" << record.z << "\t" << record.r.x << "\t" << record.r.y << "\t" << record.r.z << "\t"  << record.quality << "\t" << (int)record.button << "\n";
	}
}

void DataAcq::RegisterRawDataCallback(std::function<void(SmartScan::Point3*)> callback)
{
	mRawDataCallback = callback;
}

const std::vector<int> DataAcq::GetUsedSensors() const
{
	return mSerialBuff;
}

const int DataAcq::NUsedSensors() const
{
	return mSerialBuff.size();
}