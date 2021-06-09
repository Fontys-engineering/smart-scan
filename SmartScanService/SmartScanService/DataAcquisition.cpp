#include <iomanip>
#include <ctime>
#include <cstdio>

#include "DataAcquisition.h"
#include "Exceptions.h"

using namespace SmartScan;

DataAcq::DataAcq(bool useMockData) : mUseMockData { useMockData }, mTSCtrl(useMockData)
{

}

DataAcq::DataAcq(bool useMockData, DataAcqConfig acquisitionConfig) : mUseMockData { useMockData }, mTSCtrl(useMockData), mConfig { acquisitionConfig } 
{

}

DataAcq::~DataAcq()
{
	this->Stop(true);
}

void DataAcq::Init()
{
	if (!mUseMockData) {
		try {
			mTSCtrl.Init();
			mTSCtrl.SelectTransmitter(mConfig.transmitterID);
			mTSCtrl.SetPowerlineFrequency(mConfig.powerLineFrequency);
			mTSCtrl.SetMeasurementRate(mConfig.measurementRate);
			mTSCtrl.SetMaxRange(mConfig.maximumRange);
			mTSCtrl.SetMetric();
			mTSCtrl.SetSensorFormat();
		}
		catch (ex_trakStar e) {
			std::cerr << e.what() << std::endl;
		}
	}

	mPortNumBuff = mTSCtrl.GetAttachedPorts();
	mSerialBuff = mTSCtrl.GetAttachedSerials();

	if (mConfig.refSensorSerial >= 0) {
		for(int i = 0; i < mSerialBuff.size(); i++) {
			if (mSerialBuff[i] == mConfig.refSensorSerial) {
				refSensorPort = mPortNumBuff[i];
				mSerialBuff.erase(mSerialBuff.cbegin()+i);
				mPortNumBuff.erase(mPortNumBuff.cbegin()+i);
			}
		}
	}

	for (int i = 0; i < mPortNumBuff.size(); i++) {
		mRawBuff.push_back(std::vector<Point3>());
	}
}

void DataAcq::Start()
{
	// Check whether trak star controller has been initialised 
	if (!mRawBuff.size()) {
		throw ex_acq("[DAcq] Data acquisition is not initialized.", __func__, __FILE__);
		return;
	}

	// Check if this scan is already running:
	if (this->mRunning)	{
		return;
	}

	try	{
		this->pAcquisitionThread = std::make_unique<std::thread>(&DataAcq::DataAcquisition, this);
	}
	catch (...)	{
		throw ex_acq("[DAcq] unnable to start thread", __func__, __FILE__);
	}

	// Let it gooooo, let it gooo
	this->pAcquisitionThread->detach();

	mRunning = true;
}

void DataAcq::Stop(bool clearData)
{
	// Check if already stoppped.
	if (!this->mRunning) {
		return;
	}

	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (clearData) {
		for (int i = 0; i < mRawBuff.size(); i++) {
			mRawBuff.at(i).clear();
		}
	}

	mRunning = false;
}

const bool DataAcq::IsRunning() const
{
	return mRunning;
}

void DataAcq::DataAcquisition()
{
	// Store time on a variable time which increases the sensor sample time
	const double toRad = 3.14159265/180;
	double time = 0;
	auto startSampling = std::chrono::steady_clock::now();

	std::chrono::time_point<std::chrono::steady_clock> endSampleTime = startSampling;
	std::cout << mRawBuff.size() << std::endl;

	Point3 ref;

	while (mRunning) {
		// Store time and calculate the elapsed time since last sample
		auto startSampleTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedTime = startSampleTime - endSampleTime;

		if (elapsedTime.count() >= 1 / mConfig.measurementRate) {
            time += elapsedTime.count();

			if (mConfig.refSensorSerial >= 0) {
				ref = mTSCtrl.GetRecord(refSensorPort);
			}

            for (int i = 0; i < mPortNumBuff.size(); i++) {
                // Make Point3 obj to get the position info of the trackStar device
				Point3 raw = mTSCtrl.GetRecord(mPortNumBuff[i]); 
				button_obj.UpdateButtonState(raw.button); 
				raw.buttonState = button_obj.GetButtonState();
				// Add sample time to overal time and store in mInBuff
				raw.time = time;

				if (mConfig.refSensorSerial >= 0) {
		   	        // Check the orientation of the current point
					raw.x = raw.x - ref.x;
					raw.y = raw.y - ref.y;
					raw.z = raw.z - ref.z;

		   	        // Use the azimuth to calculate the rotation around the z-axis
		   	        //double distance = sqrt(pow(raw.x, 2) + pow(raw.y, 2));
		   	        //double angle = (atan2(raw.y, raw.x) * 180 / pi) - ref.r.z;
		   	        //raw.x = distance * cos(angle * pi/180);
		   	        //raw.y = distance * sin(angle * pi/180);
					int x_new = raw.x * cos(ref.r.z * toRad) + raw.y * sin(ref.r.z * toRad);
					int y_new = raw.y * cos(ref.r.z * toRad) - raw.x * sin(ref.r.z * toRad);

		   	        // Use the elevation to calculate the rotation around the y-axis
		   	        //distance = sqrt(pow(raw.x, 2) + pow(raw.z, 2));
		   	        //angle = (atan2(raw.z, raw.x) * 180 / pi) + ref.r.y;
		   	        //raw.x = distance * cos(angle * pi / 180);
		   	        //raw.z = distance * sin(angle * pi / 180);
					raw.x = x_new * cos(ref.r.y * toRad) + raw.z * sin(ref.r.y * toRad);
					int z_new = raw.z * cos(ref.r.y * toRad) - x_new * sin(ref.r.y * toRad);

		   	        // Use the roll difference to calculate the rotation around the x-axis
		   	        //distance = sqrt(pow(raw.y, 2) + pow(raw.z, 2));
		   	        //angle = (atan2(raw.z, raw.y) * 180 / pi) - ref.r.x;
		   	        //raw.y = distance * cos(angle * pi / 180);
		   	        //raw.z = distance * sin(angle * pi / 180);
					raw.y = y_new * cos(ref.r.x * toRad) - z_new * sin(ref.r.x * toRad);
					raw.z = z_new * cos(ref.r.x * toRad) + y_new * sin(ref.r.x * toRad);
				}
				mRawBuff[i].push_back(raw);
		    }			

			if (mRawDataCallback) {
				std::vector<Point3> sampleRow;
				for (int i = 0; i < mRawBuff.size(); i++) {
					sampleRow.push_back(mRawBuff.at(i).back());
				}
				mRawDataCallback(sampleRow);
			}

			// Store current time and calculate duration of the samples
			endSampleTime = std::chrono::steady_clock::now();
		}
    }
}

void DataAcq::RegisterRawDataCallback(std::function<void(const std::vector<Point3>&)> callback)
{
	mRawDataCallback = callback;
}

const std::vector<std::vector<Point3>>* DataAcq::getRawBuffer()
{
	return &mRawBuff;
}