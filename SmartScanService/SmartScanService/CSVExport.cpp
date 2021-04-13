#include "CSVExport.h"
#include <iostream>
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
		for (int i = 0; i < data.size(); i = i+NUsedSensors)    //Access by reference to avoid copying
		{
			for (int j = 0; j < NUsedSensors; j++)
			{
				csvFile << data.at(i + j).time << "," << data.at(i + j).x << "," << data.at(i + j).y << "," << data.at(i + j).z << "," << data.at(i + j).r.x << "," << data.at(i + j).r.y << "," << data.at(i + j).r.z << "," << data.at(i + j).quality << "," << (int)data.at(i + j).buttonState << ",";
			}
			csvFile << std::endl;
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