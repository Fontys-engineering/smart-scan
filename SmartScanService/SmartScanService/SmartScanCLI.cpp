
// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends here.
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

#include "SmartScanConfig.h"

using namespace SmartScan;

// Pre-define functions
void Usage();
void RawPrintCallback(const std::vector<SmartScan::Point3>& record);

// Create SmartScanService object
SmartScanService s3(mockMode);

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
		s3.Init(acquisitionConfig);
		s3.RegisterRawDataCallback(RawPrintCallback);
	}
	catch (ex_trakStar e) {
		std::cerr << "\t\tException thrown in TrakStar initialization: " << std::endl << "\t\t- " << e.what() << std::endl;
	}
	catch (ex_acq e) {
		std::cerr << "\t\tException thrown in Data-acquisition initialization: " << std::endl << "\t\t- " << e.what() << std::endl;
	}
	catch (ex_scan e) {
		std::cerr << "\t\tException thrown in Scan initialization: " << std::endl << "\t\t- " << e.what() << std::endl;
	}
	catch (...)	{
		std::cerr << "\t\tUnknown exception. Could not initialise Smart Scan Service" << std::endl;
		return -1;
	}

	std::cout << "\t\tFound " << s3.NumAttachedBoards() << " attached board(s)" << std::endl; 
	std::cout << "\t\tFound " << s3.NumAttachedTransmitters() << " attached transmitter(s)" << std::endl; 
	std::cout << "\t\tFound " << s3.NumAttachedSensors(true) << " attached sensor(s)" << std::endl << std::endl << std::endl;

	// Print the help screen:
	std::cout << "Welcome to the SmartScan command line application! (Type help to see a full list of commands)" << std::endl;

	char cmd[128];
	do {
		std::cout << "SmartScan>";
		std::cin.getline(cmd, 128);

		if (!strcmp(cmd, "start")) {
			try {
				s3.StartScan();

				// Print legend.
				std::cout << std::setw(4) << "Sec";
				std::cout << std::setw(4) << "But";

				for (int i = 0; i < s3.NumAttachedSensors(false); i++) {
					std::cout << std::setw(4) << 'X' << i;
					std::cout << std::setw(4) << 'Y' << i;
					std::cout << std::setw(4) << 'Z' << i;
				}
				std::cout << std::endl;
			}
			catch (ex_acq e) {
				std::cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_smartScan e) {
				std::cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (ex_scan e) {
				std::cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
			catch (...)	{
				std::cerr << "Unnable to start the scan due to an unknow error. \n";
			}
		}
		else if (!strcmp(cmd, "stop")) {
			s3.StopScan();
			std::cout << "All scans have stopped!" << std::string(70, ' ') << std::endl;
		}
		else if (!strcmp(cmd, "new")) {
            try {
				int numRefPoints;
				ScanConfig config;

				std::cout << "Welcome to the scan creation wizard!" << std::endl;
				std::cout << "Enter the number of desired reference points: " << std::flush;
				while(!(std::cin >> numRefPoints)){
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input.  Try again: ";
				}
				std::cin.ignore();

				std::cout << "Position the thumb and index finger around the reference point and press any key when ready.";
				for (int i = 0; i < numRefPoints; i++) {
					std::cin.ignore();
					Point3 PointThumb = s3.GetSingleSample(rThumbSerial);
					Point3 PointIndex = s3.GetSingleSample(rIndexSerial);
					Point3 refPoint;

					refPoint.x = ((PointThumb.x + PointIndex.x) / 2);
					refPoint.y = ((PointThumb.y + PointIndex.y) / 2);
					refPoint.z = ((PointThumb.z + PointIndex.z) / 2);

					config.refPoints.push_back(refPoint);
					std::cout << "Reference point number " << i << " set at (" << refPoint.x << ',' << refPoint.y << ',' << refPoint.z << ").";
				}

				std::cout << std::endl << "Enter the desired filtering precision (180 needs to be a multiple): " << std::flush;
				while(!(std::cin >> config.filteringPrecision)){
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input.  Try again: ";
				}
				std::cin.ignore();

				std::cout << "Enter the samplenumber after which the scan is stopped (-1 for infite duration): " << std::flush;
				while(!(std::cin >> config.stopAtSample)){
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input.  Try again: ";
				}
				std::cin.ignore();

				std::cout << "Enter the outlier point Threshold in mm: " << std::flush;
				while(!(std::cin >> config.outlierThreshold)){
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input.  Try again: ";
				}
				std::cin.ignore();

				try {
					s3.NewScan(config);
					std::cout << "New scan created" << std::endl;
				}
				catch (ex_smartScan e) {
					std::cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
				}
            }
            catch (ex_trakStar e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
            catch (ex_acq e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
            catch (ex_smartScan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
            catch (ex_scan e) {
				std::cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
			}
		}
		else if (!strcmp(cmd, "clear")) {
			s3.ClearData();
			std::cout << "All data cleared!" << std::endl;
		}
		else if (!strcmp(cmd, "delete")) {
			char ack[128];
			std::cout << "Are you sure you want to delete all? answer y/n: ";
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
			std::cout << "Scan ID\t\tNumRefs\t\tPrecision\tStopAt\t\tThreshold" << std::endl;

			for (int s = 0; s < s3.GetScansList().size(); s++) {
				std::cout << s3.GetScansList().at(s)->mId << "\t\t" << s3.GetScansList().at(s)->NumRefPoints() << "\t\t" << s3.GetScansList().at(s)->GetFilteringPrecision() << "\t\t";
				std::cout << s3.GetScansList().at(s)->GetStopAtSample() << "\t\t" << s3.GetScansList().at(s)->GetOutlierThreshold() << std::endl;
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
				s3.ExportCSV(filepath.substr(9) + ".csv", id, true);
                s3.ExportPointCloud(filepath.substr(9) + "_pc.csv", id, true);
				std::cout << "Done.\n";
			}
			catch(ex_export e) {
				std::cerr << e.what() << std::endl;
			}
			catch (...)	{
				std::cerr << "Could not export csv file" << std::endl;
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
		else if (!strcmp(cmd, "help")) {
			Usage();
		}
	} while (strcmp(cmd, "exit"));	
}

void Usage()
{
	std::cout << std::endl;
	std::cout << "Command" << "\t\t\t\t" << "Description" << std::endl;
	std::cout << "\tstart [id]\t\t\tStart the measurement. Leave id blank to start all scans." << std::endl;
	std::cout << "\tstop [id]\t\t\tStop the measurement. Leave id blank to stop all scans." << std::endl;
	std::cout << "\tnew\t\t\t\tCreate a new measurement." << std::endl;
	std::cout << "\tclear\t\t\t\tClear all recorded data." << std::endl;
	std::cout << "\tdelete [id]\t\t\tDelete a measurement. Leave id blank to delete all scans" << std::endl << "\t\t\t\t\t" << "and clear the raw data." << std::endl;
	std::cout << "\tlist\t\t\t\tPrint all the existing Scans to the console." << std::endl;
	std::cout << "\texport [id] [filename]\t\tExport the processed data of the scan id as a CSV file with" << std::endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << std::endl;
	std::cout << "\texport-raw [filename]\t\tExport the raw data of all the sensors as a CSV file with" << std::endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << std::endl;
	std::cout << "\thelp \t\t\t\tPrint this screen again." << std::endl;
	std::cout << "\texit \t\t\t\tCleanly exit the application." << std::endl;
	std::cout << std::endl;
}

void RawPrintCallback(const std::vector<SmartScan::Point3>& record)
{
	std::cout << '\r';
	std::cout << std::setw(4) << (int)record[0].time;
	std::cout << std::setw(4) << (int)record[0].buttonState;

	for (int i = 0; i < record.size(); i++) {
		std::cout << std::setw(5) << (int)record[i].x;
		std::cout << std::setw(5) << (int)record[i].y;
		std::cout << std::setw(5) << (int)record[i].z;
	}
	std::cout << ' ' << '\r' << std::flush;
}