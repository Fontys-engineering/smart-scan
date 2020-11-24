#pragma once
//this class handles the filtering of the raw scan data
//the code was exported from matlab and adapted to work with this architecture.

#include "Point3.h";
#include <vector>;

namespace SmartScan
{
	class Filtering
	{
	public:
		Filtering();

		/// <summary>
		/// process the data in place and keep only the filtered data points
		/// </summary>
		/// <param name="data"> - vector of data points as Point3 objects</param>
		void Filter(std::vector<Point3> &data);

	private:
	};
}