#include "CSVExport.h"

#include <chrono>

using namespace SmartScan;

CSVExport::CSVExport()
{

}

void CSVExport::ExportPoint3(const std::vector<Point3>& data, const std::string filename, const int NUsedSensors)
{
	csvFile.open(filename);
	time_t now = time(0);

	csvFile << data.size() << "," << NUsedSensors << std::endl;    //Add info on the first line
	if (data.size() > 0)    //Write to file:
	{
		for (auto& record : data)    //Access by reference to avoid copying
		{
			csvFile << record.time << "," << record.x << "," << record.y << "," << record.z << "," << record.r.x << "," << record.r.y << "," << record.r.z << std::endl;
		}
	}
	csvFile.close();
}

void CSVExport::ExportPC(const std::vector<Point3>& data, const std::string filename)
{
	csvFile.open(filename);
	time_t now = time(0);

	if (data.size() > 0)    //Write to file:
	{
		for (auto& record : data)    //Access by reference to avoid copying
		{
			csvFile << record.x << "," << record.y << "," << record.z << std::endl;
		}
	}
	csvFile.close();
}