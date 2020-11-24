#pragma once
//This file handles the Scan file storage and handling.

#include <vector>
#include <string>

#include "Scan.h"

namespace SmartScan
{
	class ScanDb
	{
	public:
		ScanDb();

		void SaveScan();

		/// <summary>
		/// Create a new Scan object from file (database) and return it.
		/// </summary>
		/// <param name="id"> - the id of the Scan to be returned. Leaving blank will return the latest Scan</param>
		/// <returns>Scan object</returns>
		Scan GetScan(int id = -1);

		/// <summary>
		/// Returns a list of Scan objects containing the previous scans but NO DATA POINTS
		/// To get the full scan object with all the data, use GetScan with the correct ID.
		///
		///	This function's use is to list the available scans from the database.
		/// </summary>
		/// <returns>constant vector of scan objects with empty input and output buffers</returns>
		const std::vector<Scan> GetScanList() const;
	private:
		std::string databaseJSONPath;
	};
}