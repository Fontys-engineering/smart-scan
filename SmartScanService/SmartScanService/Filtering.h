#pragma once
//this class handles the filtering of the raw scan data
//the code was exported from matlab and adapted to work with this architecture.

#include "Point3.h";
#include "ReferencePoint.h"
#include <vector>;

namespace SmartScan
{
	class Filtering
	{
	public:
		Filtering();
		Filtering(std::vector<ReferencePoint> ref_point, double phi_range, double theta_range);

		/// <summary>
		/// process the data in place and keep only the filtered data points
		/// </summary>
		/// <param name="data"> - vector of data points as Point3 objects</param>
		void Filter(std::vector<Point3> &data);

		/// <summary>
		/// process the data in place and keep only the filtered data points. use the reference data to reorientate
		/// the model if moved
		/// </summary>
		/// <param name="data"> - vector of data points as Point3 objects</param>
		/// <param name="referenceData"> - vector of reference data points as Point3 objects</param>
		void Filter(std::vector<Point3>& data, std::vector<Point3>& referenceData);

		/// <summary>
		/// Set the reference points for this filtering object
		/// </summary>
		/// <param name="referencePoints"> - vector of points of type ReferencePoint</param>
		void SetReferencePoints(std::vector<ReferencePoint> referencePoints);

		void SetResolution(double phi_range, double theta_range);

		void SetFrameSize(const unsigned int& framesize);

	private:
		//reference points (for gradient smoothing)
		std::vector<ReferencePoint> referencePoints;

		//properties:
		double phi_range = 4;
		double theta_range = 4;

		// Frame size of the measurement
		unsigned int mFramesize;

		//Filtering helper methods:
		void RotationOrientation(std::vector<Point3>& data, std::vector<Point3>& referenceData);


		void Outlier(std::vector<Point3>& data, double phi_range, double theta_range);		


		void GradientSmoothing(std::vector<Point3>& data, double phi_range, double theta_range);
	
		double findMean(double a[], int n);

		std::vector<std::vector<Point3>> CalculateCoordinates(std::vector<ReferencePoint>& ref, std::vector<Point3>& data);

		// Tests if a point is valid or not, by testing if there are other points
		// within the phiand theta range, somewhere in the array.Output is true or
		// false depending on if the point is overlapping anotherand if it's R
		// value is lower or not.
		bool TestPoint(std::vector<Point3>& data, double phi_range, double theta_range, int index);

		// Sort the data point between which reference point is closest to it. 
		// Inputs: m_data : Measurement Data
		//		   s_data : Spherical coordinate Data
		//         ref_data : Reference point Data
		std::vector<std::vector<Point3>> SortArrays(std::vector<Point3> m_data, std::vector<std::vector<Point3>> s_data, std::vector<ReferencePoint> ref_data);

		void FilterIteration(std::vector<Point3>& data, std::vector<ReferencePoint>& referencePoints, double phi_range, double theta_range);
	};
}