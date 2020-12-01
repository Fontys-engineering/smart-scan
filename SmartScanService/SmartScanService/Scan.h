#pragma once

#include "Point3.h"
#include "ReferencePoint.h"
#include "TrakStarController.h"

#include <vector>
#include <iostream>
#include<thread> 

namespace SmartScan
{
	class Scan
	{
	public:
		const int mId;

		std::vector<Point3> mInBuff;
		std::vector<Point3> mOutBuff;

		Scan(const int id);

		void Start();
		void Stop(bool clearData = false);
		double GetCompletion() const;
		bool Complete() const;
		void PostProcess();

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
#pragma endregion

	private:
		std::vector<ReferencePoint> mReferencePoints;

		//track star controller obj:
		std::unique_ptr<TrakStarController> pTSCtrl;

		//data acquisition thread:
		std::thread acquisitionThread;

		/// <summary>
		/// Polls the TrakstarController for new data, stores it and filters it.
		/// </summary>
		void DataAcquisition();

		bool mStopDataAcquisition = false;

	};
}