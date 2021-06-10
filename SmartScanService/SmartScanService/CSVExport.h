#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "Exceptions.h"
#include "Point3.h"

namespace SmartScan
{
    // This class handles the CSV file manipulation
    // i.e. import, export, formatting etc.
	class CSVExport
	{
	public:
		CSVExport();

		/// <summary>
		/// Export a Point3 vector as a CSV file.
		/// Each line coresponds to the X, Y, Z, RX, RY, RZ coordinates for a point.
		/// </summary>
		/// <param name="data"> - Reference to Point3 vector to be exported</param>
		/// <param name="filename"> - The name under which the file will be stored. If the file already exists it will be overwritten</param>
		void ExportPoint3(const std::vector<Point3>& data, const std::string filename, const int NUsedSensors);

        /// <summary>
		/// Export a Point3 vector as a CSV file.
		/// Each line coresponds to the X, Y, Z, RX, RY, RZ coordinates for a point.
		/// </summary>
		/// <param name="data"> - Reference to Point3 vector to be exported</param>
		/// <param name="filename"> - The name under which the file will be stored. If the file already exists it will be overwritten</param>
		void ExportPoint3Raw(const std::vector<std::vector<Point3>>* data, const std::string filename, bool cloud);
	private:
		std::ofstream csvFile;
	};
}