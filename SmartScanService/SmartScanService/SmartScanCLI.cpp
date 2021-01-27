
// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This provides an interface for the SmartScanServices.

//include all the libraries and sub-classes:
#include "SmartScanCLI.h"
#include "Exceptions.h"
#include "Filtering.h"

using namespace SmartScan;


int main()
{
	Filtering f;
	std::vector<Point3> m_data;


	std::cout << std::endl << "\t\t\t\t\t Smart Scan Command Line Interface Application" << std::endl << std::endl;

	//create a new SmartScanService object with mock data:
	SmartScanService s3(mockMode);

	//initialise the service:
	try {
		std::cout << "SmartScan>" << (mockMode ? "TrackSTAR device in MOCK DATA MODE" : "Initialising the TrakSTAR device. This can take up to a minute.") << std::endl;
		s3.Init();
		//register the callback (uncomment this to see a demo. Warninig: it will flood the console with values):
		//s3.RegisterNewDataCallback(TestUICallback);
		std::cout << "SmartScan>" << "TrakSTAR device initialisation done. \n";


	}
	catch (ex_trakStar& e)
	{
		std::cerr << "TrakSTAR exception: " << e.what() << " thrown in " << e.get_function() << " in " << e.get_file() << std::endl;
	}
	catch (ex_scan& e)
	{
		std::cerr << "Smart Scan Service Exception: " << e.what() << " thrown in " << e.get_function() << " in " << e.get_file() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception. Could not initialise Smart Scan Service" << std::endl;
		return -1;
	}
	//print the help screen:
	Usage();

	char cmd[128];
	do {
		std::cout << std::endl << "SmartScan>";
		std::cin.getline(cmd, 128);
		if (!strcmp(cmd, "start") || strlen(cmd) > 6 && !strncmp(cmd, "start ", 6))
		{
			int scanId = -1;
			if (strlen(cmd) > 6)
			{
				//get the id from the comand:
				std::string sCmd = cmd;
				scanId = atoi(sCmd.substr(6).c_str());
			}
			try
			{
				//start a scan using only the known good sensors:
				(scanId == -1) ? s3.StartScan(usedSensors) : s3.StartScan(scanId, usedSensors);
			}
			catch (std::exception e)
			{
				std::cerr << e.what() << std::endl;
			}
			catch (ex_trakStar e)
			{
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_smartScan e)
			{
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_scan e)
			{
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (...)
			{
				std::cerr << "Unnable to start the scan due to an unknow error. \n";
			}
		}
		else if (!strcmp(cmd, "new"))
		{
			s3.NewScan(usedSensors);
			std::cout << "New scan created" << std::endl;
		}
		else if (!strcmp(cmd, "delete"))
		{

			try {
				s3.DeleteScan();
				std::cout << "Latest scan has been deleted" << std::endl;
			}
			catch (ex_smartScan e)
			{
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (strlen(cmd) > 7 && !strncmp(cmd, "delete ", 7))
		{
			//get the id from the comand:
			std::string sCmd = cmd;
			int id = atoi(sCmd.substr(7).c_str());

			try {
				s3.DeleteScan(id);
				std::cout << "Scan " << id << " has been deleted" << std::endl;
			}
			catch (ex_smartScan e)
			{
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (!strcmp(cmd, "stop"))
		{
			s3.StopScan();
		}
		else if (!strcmp(cmd, "list"))
		{
			std::cout << "Scan ID \t Status" << std::endl;
			for (int s = 0; s < s3.GetScansList().size(); s++)
			{
				std::cout << s3.GetScansList().at(s)->mId << " \t\t " << (s3.GetScansList().at(s)->isRunning() ? "running" : "stopped") << std::endl;
			}
		}
		else if (!strcmp(cmd, "find-ref"))
		{
			s3.CalibrateReferencePoints();
		}
		else if (!strcmp(cmd, "dump"))
		{
			s3.DumpScan();
		}
		else if (strlen(cmd) > 7 && !strncmp(cmd, "export ", 7))
		{
			//cut the filepath out:
			std::string filepath = cmd;
			std::cout << "exporting to " << filepath.substr(7) << std::endl;
			try {
				s3.ExportCSV(filepath.substr(7));
				std::cout << "Done.\n";
			}
			catch (...)
			{
				std::cerr << "Could not export csv file \n";
			}
		}
		else if (strlen(cmd) > 12 && !strncmp(cmd, "point-cloud ", 12))
		{
			//cut the filepath out:
			std::string filepath = cmd;
			std::cout << "exporting to " << filepath.substr(12) << std::endl;
			try {
				s3.ExportPointCloud(filepath.substr(12));
				std::cout << "Done.\n";
			}
			catch (...)
			{
				std::cerr << "Could not export point-cloud file \n";
			}
		}
		else
		{
			Usage();
		}
	} while (strcmp(cmd, "exit"));

	////example Point3:
	//double x = 1;
	//double y = 2;
	//double z = 3;
	//Point3 exampleDataPoint(x, y, z);

	//std::cout << "Point3 Example: x=" << exampleDataPoint.x << " y=" << exampleDataPoint.y << " z=" << exampleDataPoint.z << std::endl;
}



/// <summary>
/// Display the help screen
/// </summary>
void Usage()
{
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "_____________________________________________________________HELP______________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "Measurement control" << std::endl;
	std::cout << "\t new [id] \t\t\t Create a new measurement" << std::endl;
	std::cout << "\t delete [id] \t\t\t Delete a measurement. Leave id blank to delete the last scan" << std::endl;
	std::cout << "\t start [id] \t\t\t Start the measurement or create a new one. Leave id blank to use the \n \t\t\t\t\t last scan" << std::endl;
	std::cout << "\t find-ref \t\t\t Start the routine for calibrating the reference points for the latest scan" << std::endl;
	std::cout << "\t stop [id]\t\t\t Stop the latest (running) measurement" << std::endl;
	std::cout << "\t list \t\t\t\t Print all the existing Scans to the console" << std::endl;
	std::cout << "\t dump \t\t\t\t Print all the records of the latest scan to the console (for debugging)" << std::endl;
	std::cout << "\t progress \t\t\t Get an estimate of the latest scan's completion" << std::endl;
	std::cout << "\t export [filename] \t\t Export the processed data of the latest scan as a CSV file with \n \t\t\t\t\t the given filename (no spaces allowed in the filename)" << std::endl;
	std::cout << "\t point-cloud [filename] \t Export the point-cloud data (only x,y,x) of the latest scan as \n \t\t\t\t\t a CSV file with the given filename (no spaces allowed in the filename)" << std::endl;
	std::cout << std::endl;
	std::cout << "System preferences" << std::endl;
	//std::cout << "\t calibrate \t\t Begin the glove calibration process" << std::endl;
	std::cout << std::endl;
	std::cout << "CLI usage" << std::endl;
	std::cout << "\t help \t\t\t\t print this screen again" << std::endl;
	std::cout << "\t exit \t\t\t\t cleanly exit the application" << std::endl;
	std::cout << "_______________________________________________________________________________________________________________________" << std::endl;


}

void TestUICallback(std::vector<SmartScan::Point3>& data)
{
	std::cout << "New data:" << data.back().x << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
