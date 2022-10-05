#include <ctime>
#include <cstdio>
#include <float.h>

#include "../inc/Scan.h"
#include "../inc/Exceptions.h"

using namespace SmartScan;

Scan::Scan(const int id, ScanConfig config)
	: mId { id }, mConfig { config } {
	// Resize the sorted buffer to be a 3d vector with the indices being [numRefpoints][thetaRange][phiRange]
	mSortedBuff.resize(this->NumRefPoints());
	for (int i = 0; i < mSortedBuff.size(); i++) {
		mSortedBuff[i].resize(360/mConfig.filteringPrecision);			// Theta has a default range of 0-360 degrees.
		for (int k = 0; k < mSortedBuff[i].size(); k++) {
			mSortedBuff[i][k].resize(180/mConfig.filteringPrecision);	// Phi has a default range of 0-180 degrees.
		}
	}
}

Scan::~Scan() {
	// Clear the sorted buffer.
	mSortedBuff.clear();
}

void Scan::Run() {
	// Check if the thread is already running.
	if (this->mRunning) {
		return;
	}

	// Create the Scanning thread.
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

void Scan::Stop(bool clearData) {
	// Wait a bit for the other threads to finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (clearData) {
		mLastFilteredSample = 0;

		// Replace every Point3 in the sorted buffer with an empty one.
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

void Scan::CopyOutputBuffer(std::vector<Point3>* buffer) const {
	// Loop through the sorted buffer and copy only the points that are non-empty.
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

const int Scan::NumUsedSensors() const {
	return mConfig.inBuff->size();
}

const int Scan::NumRefPoints() const {
	return mConfig.refPoints.size();
}

const int Scan::GetFilteringPrecision() const {
	return mConfig.filteringPrecision;
}

const int Scan::GetStopAtSample() const {
	return mConfig.stopAtSample;
}

const double Scan::GetOutlierThreshold() const {
	return mConfig.outlierThreshold;
}

void Scan::DataFiltering() {
	// Run while not last filtered sample is at stopAtSample or if the thread is lacking behind data acquisition.
	// -1 is there since the maximum buffer index is equal to buffer size -1.
	while (mLastFilteredSample != mConfig.stopAtSample && (mRunning || mLastFilteredSample != mConfig.inBuff->back().size() - 1)) {
		int nearestRef, nearestTheta, nearestPhi;

		// Look at the last sensor of the input buffer since if this is higher, then all the other sensors before it also have a sample.
		// +1 is here for the same reason as explained above, just the opposite way.
		if (mConfig.inBuff->back().size() > mLastFilteredSample + 1) {
			// Loop through all the sensors.
			for (int i = 0; i < mConfig.inBuff->size(); i++) {
				Point3 point = mConfig.inBuff->at(i).at(mLastFilteredSample);	// Temporarily store the sample of a single sensor at the current index.
				nearestRef = this->CalcNearestRef(&point);	// Calculate radius and find nearest reference point.

				if (point.s.r < mConfig.outlierThreshold) {	// Do not store the point if radius is too large.
					CalcAngle(mConfig.refPoints.at(nearestRef), &point); // Calculate theta and phi.

					// Calculate the indexes for the sorted buffer.
					nearestTheta = (int)(point.s.theta/mConfig.filteringPrecision);
					nearestPhi = (int)(point.s.phi/mConfig.filteringPrecision);
					
					// Do not store point if the radius is larger than the one already stored.
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

int Scan::CalcNearestRef(Point3* point) {
	int index = 0;
	float radius = DBL_MAX, temp = 0; // Set radius to be an irrealistic value.

	// Loop through all the reference points.
	for (int i = 0; i < mConfig.refPoints.size(); i++) {
		// Calulate radius and if it is smaller than the previous calculated one.
		temp = sqrt(pow(point->x - mConfig.refPoints[i].x, 2) + pow(point->y - mConfig.refPoints[i].y, 2) + pow(point->z - mConfig.refPoints[i].z, 2));
		if (temp < radius) {
			radius = temp;
			index = i;
		}
	}

	point->s.r = radius;
	return index;
}

void Scan::CalcAngle(Point3 refPoint, Point3* point) {
	point->s.theta = (atan2(point->y - refPoint.y, point->x - refPoint.x) * toAngle) + 180;
	point->s.phi = acos((point->z - refPoint.z)/point->s.r) * toAngle;
}