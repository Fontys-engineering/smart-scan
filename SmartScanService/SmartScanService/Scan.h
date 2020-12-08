#pragma once

#include "Point3.h"
#include "ReferencePoint.h"
#include "TrakStarController.h"

#include <vector>
#include <deque>
#include <iostream>
#include <thread> 
#include <chrono>

namespace SmartScan
{
	class Scan
	{
	public:
		const int mId;

		std::deque<Point3> mInBuff;
		std::deque<Point3> mOutBuff;

		Scan(const int id, TrakStarController* pTSCtrl);
		//~Scan();

		void Run();
		void Stop(bool clearData = false);
		double GetCompletion() const;
		bool Complete() const;
		void PostProcess();

		/// <summary>
		/// Dumps all samples for the inBuffer to the console
		/// </summary>
		void DumpData() const;

#pragma region reference_points

		/// <summary>
		/// Routine for finding the reference points required for the filtering algorithm
		/// </summary>
		void SetReferences();

		const std::vector<ReferencePoint>& GetReferences() const;

		/// <summary>
		/// Delete all existing reference points
		/// </summary>
		void ResetReferences();

		/// <summary>
		/// Remove an existing reference point. If the exact location of the reference is unknown (i.e. the glove is used
		///	to erase the reference) then a radius can be specified. The clossest point to the origin of the deletion, within the readius,
		///	will be removed.
		/// </summary>
		/// <param name="pos"> - The position at which the reference point is found</param>
		/// <param name="radius"> - Range of deletion </param>
		void DeleteReference(Point3 pos, double radius = 0);
#pragma endregion reference points:

	private:
#pragma region scan_properties
		double sampleRate = 50;	//in hz

#pragma endregion scan properties:

		std::vector<ReferencePoint> mReferencePoints;

		//track star controller obj:
		TrakStarController *pTSCtrl;

#pragma region data_acquisition
		//data acquisition thread:
		std::unique_ptr<std::thread> pAcquisitionThread;

		/// <summary>
		/// Polls the TrakstarController for new data, stores it and filters it.
		/// </summary>
		void DataAcquisition();
		bool mStopDataAcquisition = false;

		//filtering thread:
		std::unique_ptr<std::thread> pFilteringThread;

		void DataFiltering();
		bool mStopFiltering = false;

		int lastFilteredSample = 0;

		//timing:
		std::chrono::steady_clock::time_point lastSampleTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point scanStartTime = std::chrono::steady_clock::now();
#pragma endregion data aquisition:
	};
}