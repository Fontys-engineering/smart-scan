#include <ctime>
#include <cstdio>
#include <float.h>

#include "Scan.h"
#include "Exceptions.h"

using namespace SmartScan;

Scan::Scan(const int id, ScanConfig config)
	: mId { id }, mConfig { config }
{
	mSortedBuff.resize(this->NumRefPoints());
	for (int i = 0; i < mSortedBuff.size(); i++) {
		mSortedBuff[i].resize(360/mConfig.filteringPrecision);
		for (int k = 0; k < mSortedBuff[i].size(); k++) {
			mSortedBuff[i][k].resize(180/mConfig.filteringPrecision);
		}
	}
}

Scan::~Scan()
{
	mSortedBuff.clear();
}

void Scan::Run()
{
	if (this->mRunning) {
		return;
	}

	try	{
		this->pScanningThread = std::make_unique<std::thread>(&Scan::DataFiltering, this);
	}
	catch (...)	{
		throw ex_scan("Unnable to start thread.", __func__, __FILE__);
	}

	// Let it gooooo, let it gooo
	this->pScanningThread->detach();

	mRunning = true;
}

void Scan::Stop(bool clearData)
{
	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (clearData) {
		mLastFilteredSample = 0;

		for (int i = 0; i < mSortedBuff.size(); i++) {
			for (int k = 0; k < mSortedBuff[i].size(); k++) {
				for (int j = 0; j < mSortedBuff[i][k].size(); j++) {
					mSortedBuff[i][k][j] = Point3();
				}
			}
		}
	}

	mRunning = false;
}

const bool Scan::IsRunning() const {
	return mRunning;
}

void Scan::CopyOutputBuffer(std::vector<Point3>* buffer) const
{
	for (int i = 0; i < mSortedBuff.size(); i++) {
		for (int k = 0; k < mSortedBuff[i].size(); k++) {
			for (int j = 0; j < mSortedBuff[i][k].size(); j++) {
				if (mSortedBuff[i][k][j].s.r != DBL_MAX) {
					buffer->push_back(mSortedBuff[i][k][j]);
				}
			}
		}
	}
}

const int Scan::NumUsedSensors() const
{
	return mConfig.inBuff->size();
}

const int Scan::NumRefPoints() const
{
	return mConfig.refPoints.size();
}

const int Scan::GetFilteringPrecision() const
{
	return mConfig.filteringPrecision;
}

const int Scan::GetStopAtSample() const
{
	return mConfig.stopAtSample;
}

const double Scan::GetOutlierThreshold() const
{
	return mConfig.outlierThreshold;
}

void Scan::DataFiltering()
{
	while (mLastFilteredSample != mConfig.stopAtSample && (mRunning || mLastFilteredSample != mConfig.inBuff->back().size() - 1)) {
		int nearestRef, nearestTheta, nearestPhi;

		if (mConfig.inBuff->back().size() > mLastFilteredSample + 1) {
			for (int i = 0; i < mConfig.inBuff->size(); i++) {
				Point3 point = mConfig.inBuff->at(i).at(mLastFilteredSample);
				nearestRef = this->CalcNearestRef(&point);

				if (point.s.r < mConfig.outlierThreshold) {
					calcAngle(mConfig.refPoints.at(nearestRef), &point);

					nearestTheta = (int)(point.s.theta/mConfig.filteringPrecision);
					nearestPhi = (int)(point.s.phi/mConfig.filteringPrecision);
					
					if (point.s.r < mSortedBuff[nearestRef][nearestTheta][nearestPhi].s.r) {
						mSortedBuff[nearestRef][nearestTheta][nearestPhi] = point;
					}
				}
			}
			mLastFilteredSample++;
		}
	}

	mRunning = false;
}

int Scan::CalcNearestRef(Point3* point)
{
	int index = 0;
	float radius = DBL_MAX, temp = 0;

	for (int i = 0; i < mConfig.refPoints.size(); i++) {
		temp = sqrt(pow(point->x - mConfig.refPoints[i].x, 2) + pow(point->y - mConfig.refPoints[i].y, 2) + pow(point->z - mConfig.refPoints[i].z, 2));
		if (temp < radius) {
			radius = temp;
			index = i;
		}
	}

	point->s.r = radius;
	return index;
}

void Scan::calcAngle(Point3 refPoint, Point3* point)
{
	point->s.theta = (atan2(point->y - refPoint.y, point->x - refPoint.x) * toAngle) + 180;
	point->s.phi = acos((point->z - refPoint.z)/point->s.r) * toAngle;
}