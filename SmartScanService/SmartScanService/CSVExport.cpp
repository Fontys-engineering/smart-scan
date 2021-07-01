#include "CSVExport.h"
#include <iostream>
#include <chrono>

using namespace SmartScan;

CSVExport::CSVExport()
{

}

void CSVExport::ExportPoint3(const std::vector<Point3>* data, const std::string filename)
{
	csvFile.open(filename);

	csvFile << data->size() << std::endl;
	if (!data->empty())	{
		for (int i = 0; i < data->size(); i = i++) {
			csvFile << data->at(i).time << "," << data->at(i).x << "," << data->at(i).y << "," << data->at(i).z << "," << data->at(i).r.x << "," << data->at(i).r.y << "," << data->at(i).r.z << "," << data->at(i).quality << "," << (int)data->at(i).buttonState << std::endl;
		}
	}
	else {
		throw ex_export("Raw buffer is empty.", __func__, __FILE__);
	}
	csvFile.close();
}

void CSVExport::ExportPoint3Cloud(const std::vector<Point3>* data, const std::string filename)
{
	csvFile.open(filename);

	csvFile << 'X' << ',' << 'Y' << ',' << 'Z' << ',' << "Button" << std::endl;
	if (!data->empty())	{
		for (int i = 0; i < data->size(); i = i++) {
			csvFile << data->at(i).x << "," << data->at(i).y << "," << data->at(i).z << std::endl;
		}
	}
	else {
		throw ex_export("Raw buffer is empty.", __func__, __FILE__);
	}
	csvFile.close();
}

void CSVExport::ExportPoint3Raw(const std::vector<std::vector<Point3>>* data, const std::string filename)
{
	csvFile.open(filename);

	csvFile << data->at(0).size() << "," << data->size() << std::endl;
	if (!data->at(0).empty()) {
		for (int i = 0; i < data->at(0).size(); i++)
		{
			for (int j = 0; j < data->size(); j++) {
				csvFile << data->at(j).at(i).time << "," << data->at(j).at(i).x << "," << data->at(j).at(i).y << "," << data->at(j).at(i).z << "," << data->at(j).at(i).r.x << "," << data->at(j).at(i).r.y << "," << data->at(j).at(i).r.z << "," << data->at(j).at(i).quality << "," << (int)data->at(j).at(i).buttonState << ",";
			}
			csvFile << std::endl;
		}
	}
	else {
		throw ex_export("Raw buffer is empty.", __func__, __FILE__);
	}

	csvFile.close();
}

void CSVExport::ExportPoint3RawCloud(const std::vector<std::vector<Point3>>* data, const std::string filename)
{
	csvFile.open(filename);

	csvFile << 'X' << ',' << 'Y' << ',' << 'Z' << ',' << "Button" << std::endl;
	if (!data->at(0).empty()) {
		for (int j = 0; j < data->size(); j++)
		{
			for (int i = 0; i < data->at(j).size(); i++) {
				csvFile << data->at(j).at(i).x << "," << data->at(j).at(i).y << "," << data->at(j).at(i).z << std::endl; 
			}
		}
	}
	else {
		throw ex_export("Raw buffer is empty.", __func__, __FILE__);
	}

	csvFile.close();
}