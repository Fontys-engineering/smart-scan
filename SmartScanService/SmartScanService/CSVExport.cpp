#include "CSVExport.h"
#include <chrono>

using namespace SmartScan;

CSVExport::CSVExport()
{

}

void CSVExport::ExportPoint3(const std::vector<Point3>& data, const std::string filename)
{
	csvFile.open(filename);
	time_t now = time(0);
	//add info on the first line
	csvFile << data.size() << std::endl;
	//write to file:
	if (data.size() > 0)
	{
		for (auto& record : data) // access by reference to avoid copying
		{
			csvFile << record.x << "," << record.y << "," << record.z << "," << record.r.x << "," << record.r.y << "," << record.r.z << std::endl;
		}
	}
	csvFile.close();
}

void CSVExport::ExportPC(const std::vector<Point3>& data, const std::string filename)
{
	csvFile.open(filename);
	time_t now = time(0);
	//write to file:
	if (data.size() > 0)
	{
		for (auto& record : data) // access by reference to avoid copying
		{
			csvFile << record.x << "," << record.y << "," << record.z << std::endl;
		}
	}
	csvFile.close();
}