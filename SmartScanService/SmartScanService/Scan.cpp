#include "Scan.h"
#include "Exceptions.h"
#include <iomanip>
#include <ctime>
#include <cstdio>

using namespace SmartScan;

Scan::Scan(const int id, std::vector<std::vector<Point3>*> rawDataBuffer, std::vector<Point3> refPoints, int filteringPrecision)
	: mId { id }, mInBuff { rawDataBuffer }, mFilteringPrecision { filteringPrecision }
{

}

Scan::~Scan()
{
	this->Stop(true);
}

void Scan::Run()
{
	// Check if reference points have been defined:
	mRunning = true;

	try
	{
		this->pScanningThread = std::make_unique<std::thread>(&Scan::DataFiltering, this);
	}
	catch (...)
	{
		throw ex_scan("Unnable to start thread.", __func__, __FILE__);
	}

	// Let it gooooo, let it gooo
	this->pScanningThread->detach();

	mRunning = true;
}

void Scan::Stop(bool clearData)
{
	// Check if scanning thread is already stoppped.
	if (!this->mRunning) {
		return;
	}

	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if (clearData) {
		for (int i = 0; i < mSortedBuff.size(); i++) {
			for (int k = 0; k < mSortedBuff[i].size(); k++) {
				mSortedBuff[i][k].clear();
			}
		}
	}

	mRunning = false;
}

const bool Scan::IsRunning() const
{
	return mRunning;
}

// TODO: fix processing the remaining data after acquisition stops when filtering has fallen behind
void Scan::DataFiltering()
{
	while (mRunning)
	{

	}
}

const int SmartScan::Scan::NumUsedSensors() const
{
	return mInBuff.size();
}

const int SmartScan::Scan::NumRefPoints() const
{
	return mRefPoints.size();
}

const double SmartScan::Scan::GetFilteringPrecision() const
{
	return mFilteringPrecision;
}