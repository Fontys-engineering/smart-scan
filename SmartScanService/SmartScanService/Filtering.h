#pragma once

#include <vector>

#include "Point3.h"

namespace SmartScan
{
    // This class handles the filtering of the raw scan data.
    // The code was exported from matlab and adapted to work with this architecture.
	class Filtering
	{
	public:
		Filtering();
		Filtering(std::vector<Point3> ref_point, double phi_range, double theta_range);

		/// <summary>
		/// Process the data in place and keep only the filtered data points. 
        /// Use the reference data to reorientate the model if moved.
		/// </summary>
		/// <param name="data"> - Vector of data points as Point3 objects.</param>
		/// <param name="referenceData"> - Vector of reference data points as Point3 objects.</param>
		void Filter(std::vector<Point3> &data);
		void Filter(std::vector<Point3>& data, std::vector<Point3>& referenceData);

		/// <summary>
		/// Set the reference points for this filtering object.
		/// </summary>
		/// <param name="referencePoints"> - Vector of points of type ReferencePoint.</param>
		void SetReferencePoints(std::vector<Point3> referencePoints);

		void SetPrecision(double phi_range, double theta_range);

		void SetFrameSize(const unsigned int& framesize);
	private:
		double phi_range;                               // Properties.
		double theta_range;                             // Properties.
		unsigned int mFramesize;                        // Frame size of the measurement.
		std::vector<Point3> referencePoints;    // Reference points (for gradient smoothing).

        /// <summary>
        /// RotationOrientation calculates new x, y and z values based on the azimuth, elevation and roll values.
        /// </summary>
		void RotationOrientation(std::vector<Point3>& data, std::vector<Point3>& referenceData);

		void Outlier(std::vector<Point3>& data, double phi_range, double theta_range);		

		void GradientSmoothing(std::vector<Point3>& data, double phi_range, double theta_range);
	
		double findMean(double a[], int n);

		std::vector<std::vector<Point3>> CalculateCoordinates(std::vector<Point3>& ref, std::vector<Point3>& data);

        /// <summary>
		/// Tests if a point is valid or not, by testing if there are other points
		/// within the phiand theta range, somewhere in the array. Output is true or
		/// false depending on if the point is overlapping anotherand if it's R
		/// value is lower or not.
        /// </summary>
		bool TestPoint(std::vector<Point3>& data, double phi_range, double theta_range, int index);

        /// <summary>
		/// Sort the data point between which reference point is closest to it. 
        /// </summary>
		/// <param name="m_data"> - Measurement data.</param>
		/// <param name="s_data"> - Spherical coordinates data.</param>
		/// <param name="ref_data"> - Reference point data.</param>
		std::vector<std::vector<Point3>> SortArrays(std::vector<Point3> m_data, std::vector<std::vector<Point3>> s_data, std::vector<Point3> ref_data);

		void FilterIteration(std::vector<Point3>& data, std::vector<Point3>& referencePoints, double phi_range, double theta_range);
	};
}