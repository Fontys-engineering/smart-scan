#pragma once
//This is the main smart scan library class
//It makes use of the Filtering, TrackStarController, Trigger and CSVExport sub-classes
//
//Applications using SmartScan software should use this class and not the other classes directly.


#include "Point3.h";
#include "ReferencePoint.h";
#include "Scan.h"

#include <vector>

namespace SmartScan
{
	class SmartScanService
	{
	public:
		///example comment

		SmartScanService();

		/// <summary>
		/// Initialise the Smart Scan service, Trak star device etc.
		/// </summary>
		void Init();

		/// <summary>
		/// Routine for calibrating the position of the sensors relative to the fingertips
		/// </summary>
		void CalibrateSensors();

#pragma region reference_points
		
		/// <summary>
		/// Routine for finding the reference points required for the filtering algorithm
		/// </summary>
		void SetReferences();

		std::vector<ReferencePoint> GetReferences();

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

#pragma region scan
		std::vector
#pragma endregion
	
	private:
		std::vector<ReferencePoint> referencePoints;
	};
}