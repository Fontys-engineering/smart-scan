
// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
// This provides an interface for the SmartScanServices.

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include "SmartScanCLI.h"
#include "Exceptions.h"
#include "Filtering.h"

using namespace SmartScan;

#include <iomanip>

int main()
{
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
	std::cout << "                /$$$$$$                                      /$$      /$$$$$$                               " << std::endl;
	std::cout << "               /$$__  $$                                    | $$     /$$__  $$                              " << std::endl;
	std::cout << "              | $$  \\__/ /$$$$$$/$$$$   /$$$$$$   /$$$$$$  /$$$$$$  | $$  \\__/  /$$$$$$$  /$$$$$$  /$$$$$$$ " << std::endl;
	std::cout << "              |  $$$$$$ | $$_  $$_  $$ |____  $$ /$$__  $$|_  $$_/  |  $$$$$$  /$$_____/ |____  $$| $$__  $$" << std::endl;
	std::cout << "               \\____  $$| $$ \\ $$ \\ $$  /$$$$$$$| $$  \\__/  | $$     \\____  $$| $$        /$$$$$$$| $$  \\ $$" << std::endl;
	std::cout << "               /$$  \\ $$| $$ | $$ | $$ /$$__  $$| $$        | $$ /$$ /$$  \\ $$| $$       /$$__  $$| $$  | $$" << std::endl;
	std::cout << "              |  $$$$$$/| $$ | $$ | $$|  $$$$$$$| $$        |  $$$$/|  $$$$$$/|  $$$$$$$|  $$$$$$$| $$  | $$" << std::endl;
	std::cout << "               \\______/ |__/ |__/ |__/ \\_______/|__/         \\___/   \\______/  \\_______/ \\_______/|__/  |__/" << std::endl;
	std::cout << std::endl << "\t\t\t\t Changing orthopaedic footwear from an art to a science" << std::endl << std::endl << std::endl << std::endl;
	std::cout << std::endl << "\t\tInitializing..." << std::endl;

	// Initialise the service:
	try {
		s3.Init();
		s3.RegisterRawDataCallback(RawPrintCallback);
	}
	catch (ex_trakStar& e)
	{
		std::cerr << "\nTrakSTAR exception: " << e.what() << "\nthrown in " << e.get_function() << " in " << e.get_file() << std::endl << std::endl;
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

	std::cout << "\t\tFound " << s3.NumAttachedBoards() << " attached board(s)" << std::endl; 
	std::cout << "\t\tFound " << s3.NumAttachedTransmitters() << " attached transmitter(s)" << std::endl; 
	std::cout << "\t\tFound " << s3.NumAttachedSensors(true) << " attached sensor(s)" << std::endl << std::endl << std::endl;

	// Print the help screen:
	std::cout << "Welcome to the SmartScan command line application! (Type help to see a full list of commands)" << std::endl;

	char cmd[128];
	do {
        std::cout << std::string(30, ' ') << '\r';
		std::cout << "SmartScan>";
		std::cin.getline(cmd, 128);

		if (!strcmp(cmd, "start")) {
			try {
				s3.StartScan();
			}
			catch (ex_trakStar e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_smartScan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_scan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (...)	{
				std::cerr << "Unnable to start the scan due to an unknow error. \n";
			}

			std::cout << std::endl;
			std::cout << std::setw(4) << "Sec";
			std::cout << std::setw(4) << "But";

			for (int i = 0; i < s3.NumAttachedSensors(false); i++) {
				std::cout << std::setw(4) << 'X' << i;
				std::cout << std::setw(4) << 'Y' << i;
				std::cout << std::setw(4) << 'Z' << i;
			}
			std::cout << std::endl;

		}
		else if (!strcmp(cmd, "stop")) {
			try {
				s3.StopScan();
				std::cout << "All scans have stopped!" << std::string(50, ' ') << std::endl << std::endl;
			}
			catch (ex_smartScan e) {
				std::cout << std::endl;
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (!strcmp(cmd, "new")) {
            try {
                s3.NewScan(config);
            }
            catch (ex_smartScan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			std::cout << "New scan created" << std::endl;
		}
		else if (!strcmp(cmd, "delete")) {
			char ack[128];
			std::cout << "Are you sure you want to delete all? answer y/n: " << std::endl;
			std::cin.getline(ack,128);

			if (!strncmp(ack,"y", 1)) {
				try {
					s3.DeleteScan();

					std::cout << "Deleted all scans!" << std::endl;
				}
				catch (ex_smartScan e) {
					std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
				}
			}
		}
		else if (strlen(cmd) > 7 && !strncmp(cmd, "delete ", 7)) {
			// Get the id from the comand:
			std::string sCmd = cmd;
			int id = atoi(sCmd.substr(7).c_str());

			try {
				s3.DeleteScan(id);
				std::cout << "Scan " << id << " has been deleted" << std::endl;
			}
			catch (ex_smartScan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (!strcmp(cmd, "list")) {
			std::cout << "Scan ID \t Status" << std::endl;

			for (int s = 0; s < s3.GetScansList().size(); s++) {
				std::cout << s3.GetScansList().at(s)->mId << " \t\t " << (s3.GetScansList().at(s)->IsRunning() ? "running" : "stopped") << std::endl;
			}
		}
		else if (strlen(cmd) > 9 && !strncmp(cmd, "find-ref ", 9)) {
			// Get the id from the comand:
			std::string sCmd = cmd;
			int id = atoi(sCmd.substr(9).c_str());

			s3.CalibrateReferencePoints(id);
		}
		else if (strlen(cmd) > 5 && !strncmp(cmd, "dump ", 5)) {
			// Get the id from the comand:
			std::string sCmd = cmd;
			int id = atoi(sCmd.substr(5).c_str());

			try {
				std::cout << "Dump values of scan: " << id << std::endl;
				s3.DumpScan(id);
			}
			catch (ex_smartScan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (strlen(cmd) > 7 && !strncmp(cmd, "export ", 7)) {
			// Get the id from the comand:
			std::string sCmd = cmd;
			int id = atoi(sCmd.substr(7, 1).c_str());

			// Cut the filepath out:
			std::string filepath = cmd;
			std::cout << "exporting raw data from scan: " << id << " into file: " << filepath.substr(9) << std::endl;

			try {
				s3.ExportCSV(filepath.substr(9), id);
				std::cout << "Done.\n";
			}
			catch (...)	{
				std::cerr << "Could not export csv file \n";
			}
		}
		else if (strlen(cmd) > 11 && !strncmp(cmd, "export-raw ", 11)) {
			// Get the id from the comand:
			std::string sCmd = cmd;

			// Cut the filepath out:
			std::string filepath = cmd;
			std::cout << "Exporting raw data into file: " << filepath.substr(11) << std::endl;

			try {
				s3.ExportCSV(filepath.substr(11) + ".csv", 0, true);
                s3.ExportPointCloud(filepath.substr(11) + "_pc.csv", 0, true);
				std::cout << "Done.\n";
			}
			catch(ex_export e) {
				std::cerr << e.what() << std::endl;
			}
			catch (...)	{
				std::cerr << "Could not export csv file" << std::endl;
			}
		}
	} while (strcmp(cmd, "exit"));	
}

void Usage()
{
	std::cout << std::endl;
	std::cout << "__________________________________________________________HELP_________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "Measurement control" << std::endl;
	std::cout << "\t new \t\t\t\t Create a new measurement" << std::endl;
	std::cout << "\t delete [id] \t\t\t Delete a measurement. Leave id blank to delete all the scans." << std::endl;
	std::cout << "\t start [id] \t\t\t Start the measurement. Leave id blank to start all scans." << std::endl;
	std::cout << "\t find-ref [id] \t\t\t Start the routine for calibrating the reference points for the specified scan." << std::endl;
	std::cout << "\t stop [id]\t\t\t Stop the measurement. Leave id blank to stop all scans." << std::endl;
	std::cout << "\t list \t\t\t\t Print all the existing Scans to the console." << std::endl;
	std::cout << "\t dump [id] \t\t\t Print all the records of the scan id to the console." << std::endl;
	std::cout << "\t export [id] [filename] \t Export the processed data of the scan id as a CSV file with \n \t\t\t\t\t the given filename (no spaces allowed in the filename)." << std::endl;
	std::cout << "\t export-raw [filename] \t\t Export the raw data of all the sensors as a CSV file with \n \t\t\t\t\t the given filename (no spaces allowed in the filename)." << std::endl;
	std::cout << std::endl;
	std::cout << "CLI usage" << std::endl;
	std::cout << "\t help \t\t\t\t print this screen again" << std::endl;
	std::cout << "\t exit \t\t\t\t cleanly exit the application" << std::endl;
	std::cout << "_______________________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
}

void RawPrintCallback(const std::vector<SmartScan::Point3>& record)
{
	std::cout << std::setw(4) << (int)record[0].time;
	std::cout << std::setw(4) << (int)record[0].buttonState;

	for (int i = 0; i < record.size(); i++) {
		std::cout << std::setw(5) << (int)record[i].x;
		std::cout << std::setw(5) << (int)record[i].y;
		std::cout << std::setw(5) << (int)record[i].z;
	}
	std::cout << '\r' << std::flush;
}