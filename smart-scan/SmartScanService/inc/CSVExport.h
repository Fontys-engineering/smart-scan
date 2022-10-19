// This is the SmartScan CSVexport class.
// This class handles the CSV file manipulation i.e. import, export, formatting etc.

#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "Exceptions.h"
#include "Point3.h"

namespace SmartScan {
	class CSVExport
	{
	public:
		// Constructor. Creates a CSVExport object that handles everything exporting related.
		CSVExport();

		// Export a Point3 vector to a CSV file in the MATLAB format (time, position, rotation, quality and button).
		// Arguments:
		// - data : constant pointer to the Point3 vector (Read only). 
		// - filename : constant string containing the name of the exported file. 
		void ExportPoint3(const std::vector<Point3>* data, const std::string filename);

		// Export a Point3 vector to a CSV file in the CloudCompare format (position only).
		// Arguments:
		// - data : constant pointer to the Point3 vector (Read only). 
		// - filename : constant string containing the name of the exported file. 
		void ExportPoint3Cloud(const std::vector<Point3>* data, const std::string filename);

		// Export the raw data buffer to a CSV file in the MATLAB format (time, position, rotation, quality and button).
		// Every sensor will have its own columns. (1 sensor 9 columns, 2 has 18 etc.)
		// Arguments:
		// - data : constant pointer to the raw data buffer (Read only). 
		// - filename : constant string containing the name of the exported file. 
		void ExportPoint3Raw(const std::vector<std::vector<Point3>>* data, const std::string filename);

		// Export the raw data buffer to a CSV file in the CloudCompare format (position only).
		// Arguments:
		// - data : constant pointer to the raw data buffer (Read only). 
		// - filename : constant string containing the name of the exported file. 
		void ExportPoint3RawCloud(const std::vector<std::vector<Point3>>* data, const std::string filename);
	
	private:
		std::ofstream csvFile;				// Output file object.
	};
}