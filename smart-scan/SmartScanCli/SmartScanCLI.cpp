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
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include "SmartScanService.h"
#include "DirDef.h"
#include "ini.h"

using namespace std;
using namespace SmartScan;
namespace fs = std::filesystem;

// Glove setup
bool initDone = false;

// Pre-define functions
void Usage();
void RawPrintCallback(const vector<Point3>& record);
int SensorIdentification(const char* sensorParam);
int GetSensorID(const char* parameter);

// Create SmartScanService object
SmartScanService s3;
DataAcqConfig acquisitionConfig;
fs::path settingsPath(DIR_SETTINGS);
ini::File iniFile;

int main() {
	// Print welcome screen.
	cout << endl << endl << endl << endl << endl << endl << endl << endl << endl;
	cout << "                /$$$$$$                                      /$$      /$$$$$$                               " << endl;
	cout << "               /$$__  $$                                    | $$     /$$__  $$                              " << endl;
	cout << "              | $$  \\__/ /$$$$$$/$$$$   /$$$$$$   /$$$$$$  /$$$$$$  | $$  \\__/  /$$$$$$$  /$$$$$$  /$$$$$$$ " << endl;
	cout << "              |  $$$$$$ | $$_  $$_  $$ |____  $$ /$$__  $$|_  $$_/  |  $$$$$$  /$$_____/ |____  $$| $$__  $$" << endl;
	cout << "               \\____  $$| $$ \\ $$ \\ $$  /$$$$$$$| $$  \\__/  | $$     \\____  $$| $$        /$$$$$$$| $$  \\ $$" << endl;
	cout << "               /$$  \\ $$| $$ | $$ | $$ /$$__  $$| $$        | $$ /$$ /$$  \\ $$| $$       /$$__  $$| $$  | $$" << endl;
	cout << "              |  $$$$$$/| $$ | $$ | $$|  $$$$$$$| $$        |  $$$$/|  $$$$$$/|  $$$$$$$|  $$$$$$$| $$  | $$" << endl;
	cout << "               \\______/ |__/ |__/ |__/ \\_______/|__/         \\___/   \\______/  \\_______/ \\_______/|__/  |__/" << endl;
	cout << endl << "\t\t\t\t Changing orthopaedic footwear from an art to a science" << endl << endl << endl << endl;
	cout << endl << "\t\tInitializing..." << endl;

	// Setup directories
	fs::create_directories(DIR_FILTER_PLUGINS);
	fs::create_directories(DIR_CONFIG);
	fs::create_directories(DIR_TEMP);
	fs::create_directories(DIR_SCANS);

	// Read system settings and save the settings to data-acquisition configuration
	do {
		if (fs::exists(DIR_SETTINGS)) {
			// Setup ini file
			iniFile = ini::open(DIR_SETTINGS);

			// Setup system section
			if (!iniFile.has_section(SECTION_SYSTEM)) {
				iniFile.add_section(SECTION_SYSTEM);
				iniFile.write(DIR_SETTINGS);
				initDone = false;
			}

			if (iniFile.has_section(SECTION_SYSTEM)) {
				// Setup transmitter ID parameter
				if (!iniFile[SECTION_SYSTEM].has_key(PARAM_SYS_TRANS_ID)) {
					iniFile[SECTION_SYSTEM].set<int>(PARAM_SYS_TRANS_ID, 0);
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.transmitterID = (short)iniFile[SECTION_SYSTEM].get<int>(PARAM_SYS_TRANS_ID);
				cout << "\t\t\tTransmitter ID set to: " << acquisitionConfig.transmitterID << endl;

				// Setup max range
				if (!iniFile[SECTION_SYSTEM].has_key(PARAM_SYS_MAX_RANGE)) {
					iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_MAX_RANGE, 36.0);
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.maximumRange = iniFile[SECTION_SYSTEM].get<double>(PARAM_SYS_MAX_RANGE);
				cout << "\t\t\tMax range set to: " << acquisitionConfig.transmitterID * 2.54 << " cm" << endl;

				// Setup measurement rate
				if (!iniFile[SECTION_SYSTEM].has_key(PARAM_SYS_MEAS_RATE)) {
					/*cout << "\t\tPlease enter the desired sample rate (between 20 and 255, default = 250) >" << flush;
					while (!(cin >> acquisitionConfig.measurementRate)) { // Make sure the user can only input numbers.
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
						cout << "\t\t\tInvalid input. Try again: ";
					}
					cin.ignore();*/
					acquisitionConfig.measurementRate = 250;

					if (acquisitionConfig.measurementRate > 255) {
						iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_MEAS_RATE, 255);
					}
					else if (acquisitionConfig.measurementRate < 20) {
						iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_MEAS_RATE, 20);
					}
					else {
						iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_MEAS_RATE, acquisitionConfig.measurementRate);
					}
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.measurementRate = iniFile[SECTION_SYSTEM].get<double>(PARAM_SYS_MEAS_RATE);
				cout << "\t\t\tMeasurement rate set to: " << acquisitionConfig.measurementRate << " Hz" << endl;

				// Setup powerline frequency
				if (!iniFile[SECTION_SYSTEM].has_key(PARAM_SYS_PWR_LINE_FREQ)) {
					cout << "\t\tPlease enter the desired power line frequency (50 or 60) >" << flush;
					while (!(cin >> acquisitionConfig.powerLineFrequency)) { // Make sure the user can only input numbers.
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
						cout << "\t\t\tInvalid input. Try again >";
					}
					cin.ignore();

					if (acquisitionConfig.powerLineFrequency != 50 && acquisitionConfig.powerLineFrequency != 60) {
						iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_PWR_LINE_FREQ, 60);
					}
					else {
						iniFile[SECTION_SYSTEM].set<double>(PARAM_SYS_PWR_LINE_FREQ, acquisitionConfig.powerLineFrequency);
					}
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.powerLineFrequency = iniFile[SECTION_SYSTEM].get<double>(PARAM_SYS_PWR_LINE_FREQ);
				cout << "\t\t\tPower line frequency set to: " << acquisitionConfig.powerLineFrequency << " Hz" << endl;
				initDone = true;
			}

			if (!iniFile.has_section(SECTION_SENSOR)) {
				iniFile.add_section(SECTION_SENSOR);
				iniFile.write(DIR_SETTINGS);
				initDone = false;
			}

			if (iniFile.has_section(SECTION_SENSOR)) {
				// Setup frame rotations
				if (!iniFile[SECTION_SENSOR].has_key(PARAM_SNSR_AZIMUTH_OFFSET)) {
					iniFile[SECTION_SENSOR].set<double>(PARAM_SNSR_AZIMUTH_OFFSET, 0);
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.frameRotations[0] = iniFile[SECTION_SENSOR].get<double>(PARAM_SNSR_AZIMUTH_OFFSET);
				cout << "\t\t\tSensor's azimuth offset is set to " << acquisitionConfig.frameRotations[0] << " degrees" << endl;

				if (!iniFile[SECTION_SENSOR].has_key(PARAM_SNSR_ELEVATION_OFFSET)) {
					iniFile[SECTION_SENSOR].set<double>(PARAM_SNSR_ELEVATION_OFFSET, 0);
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.frameRotations[1] = iniFile[SECTION_SENSOR].get<double>(PARAM_SNSR_ELEVATION_OFFSET);
				cout << "\t\t\tSensor's elevation offset is set to " << acquisitionConfig.frameRotations[1] << " degrees" << endl;

				if (!iniFile[SECTION_SENSOR].has_key(PARAM_SNSR_ROLL_OFFSET)) {
					iniFile[SECTION_SENSOR].set<double>(PARAM_SNSR_ROLL_OFFSET, 0);
					iniFile.write(DIR_SETTINGS);
				}
				acquisitionConfig.frameRotations[2] = iniFile[SECTION_SENSOR].get<double>(PARAM_SNSR_ROLL_OFFSET);
				cout << "\t\t\tSensor's roll offset is set to " << acquisitionConfig.frameRotations[2] << " degrees" << endl;
				initDone = true;
			}
		}
		else {
			// Create settings INI
			fstream settingsINI(DIR_SETTINGS);
			settingsINI.open(DIR_SETTINGS, ios::in | ios::out | ios::trunc);
			settingsINI.close();
			initDone = false;
		}
	} while (!initDone);

	// Initialise the service:
	try {
		s3.Init(acquisitionConfig);
		s3.RegisterRawDataCallback(RawPrintCallback);
	}
	catch (ex_trakStar e) {
		cerr << "\t\tException thrown in TrakStar initialization: " << endl << "\t\t- " << e.what() << endl;
	}
	catch (ex_acq e) {
		cerr << "\t\tException thrown in Data-acquisition initialization: " << endl << "\t\t- " << e.what() << endl;
	}
	catch (ex_scan e) {
		cerr << "\t\tException thrown in Scan initialization: " << endl << "\t\t- " << e.what() << endl;
	}
	catch (...)	{
		cerr << "\t\tUnknown exception. Could not initialise Smart Scan Service" << endl;
		return -1;
	}

	// Print general info.
	cout << "\t\tFound " << s3.NumAttachedBoards() << " attached board(s)" << endl; 
	cout << "\t\tFound " << s3.NumAttachedTransmitters() << " attached transmitter(s)" << endl; 
	cout << "\t\tFound " << s3.NumAttachedSensors(true) << " attached sensor(s)" << endl;

	acquisitionConfig.refSensorSerial = SensorIdentification(PARAM_SNSR_REF_SERIAL);
	acquisitionConfig.thumbSensorSerial = SensorIdentification(PARAM_SNSR_THUMB_SERIAL);
	acquisitionConfig.indexSensorSerial = SensorIdentification(PARAM_SNSR_INDEX_SERIAL);
	acquisitionConfig.middleSensorSerial = SensorIdentification(PARAM_SNSR_MIDDLE_SERIAL);

	s3.SensorSetup(acquisitionConfig);

	// Print the help screen:
	cout << "Welcome to the SmartScan command line application! (Type help to see a full list of commands)" << endl;

	char cmd[128];
	do {
		// Print prompt.
		cout << "SmartScan>";

		// Wait for user input and store it in cmd.
		cin.getline(cmd, 128);

		// Create a new scan.
		if (!strcmp(cmd, "new")) {
			try {
				int numRefPoints;
				ScanConfig config;

				cout << "Welcome to the scan creation wizard!" << endl;
				cout << "Enter the number of desired reference points: " << flush;
				while (!(cin >> numRefPoints)) { // Make sure the user can only input numbers.
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid input.  Try again: ";
				}
				cin.ignore();

				cout << "Position the thumb and index finger around the reference point and press any key when ready.";
				for (int i = 0; i < numRefPoints; i++) { // Collect the same amount of reference points as specified earlier.
					cin.ignore();
					Point3 PointThumb = s3.GetSingleSample(GetSensorID(PARAM_SNSR_THUMB_SERIAL));
					Point3 PointIndex = s3.GetSingleSample(GetSensorID(PARAM_SNSR_INDEX_SERIAL));
					Point3 refPoint;

					// Average every coordinate to get the point in between the two fingers.
					refPoint.x = ((PointThumb.x + PointIndex.x) / 2);
					refPoint.y = ((PointThumb.y + PointIndex.y) / 2);
					refPoint.z = ((PointThumb.z + PointIndex.z) / 2);

					config.refPoints.push_back(refPoint);
					cout << "Reference point number " << i << " set at (" << refPoint.x << ',' << refPoint.y << ',' << refPoint.z << ").";
				}

				cout << endl << "Enter the desired filtering precision (180 needs to be a multiple): " << flush;
				while (!(cin >> config.filteringPrecision)) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid input.  Try again: ";
				}
				cin.ignore();

				try {
					s3.NewScan(config);
					cout << "New scan created" << endl;
				}
				catch (ex_smartScan e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
			}
			catch (ex_trakStar e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
			catch (ex_acq e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
			catch (ex_smartScan e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
			catch (ex_scan e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
		}
		// Calibrate the sensor offset with respect to the glove and finger t h i c c n e s s.
		// This function probably still needs adjustsments or a better implementation.
		else if (!strcmp(cmd, "calibrate")) {
			try {
				cout << "Put your hand on the table of the transmitter and press enter when ready." << flush;
				cin.ignore();

				s3.CorrectZOffset(GetSensorID(PARAM_SNSR_THUMB_SERIAL));
				s3.CorrectZOffset(GetSensorID(PARAM_SNSR_INDEX_SERIAL));
				s3.CorrectZOffset(GetSensorID(PARAM_SNSR_MIDDLE_SERIAL));

				cout << "New offset calculated succesfully!" << endl;
			}
			catch (ex_trakStar e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
			catch (ex_acq e) {
				cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
			}
		}
		// Print 1 point of all sensors
		else if (!strcmp(cmd, "highest")) {
			int serial = s3.HighestSensor();
		}
		// List all the created scans and its options.
		else if (!strcmp(cmd, "list")) {
			cout << "Scan ID\t\tNumRefs\t\tPrecision\tStopAt" << endl;//\t\tThreshold" << endl;

			for (int s = 0; s < s3.GetScansList().size(); s++) {
				cout << s3.GetScansList().at(s)->mId << "\t\t" << s3.GetScansList().at(s)->NumRefPoints() << "\t\t" << s3.GetScansList().at(s)->GetFilteringPrecision() << "\t\t";
				cout << s3.GetScansList().at(s)->GetStopAtSample() << endl;  //<< "\t\t" << s3.GetScansList().at(s)->GetOutlierThreshold() << endl;
			}
		}
		// Print the help menu.
		else if (!strcmp(cmd, "help")) {
			Usage();
		}
		// Clear all recorded data.
		else if (!strcmp(cmd, "clear")) {
			s3.ClearData();
			cout << "All data cleared!" << endl;
		}
		// These commands are only allowed when there is a scan created
		else if (s3.NumOfScans() > 0) {
			// Start collecting data.
			if (!strcmp(cmd, "start")) {
				try {
					s3.StartScan();

					// Print legend.
					cout << setw(4) << "Sec" << setw(4) << "But";

					for (int i = 0; i < s3.NumAttachedSensors(false); i++) {
						cout << setw(4) << 'X' << i << setw(4) << 'Y' << i << setw(4) << 'Z' << i;
					}
					cout << endl;
				}
				catch (ex_acq e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (ex_smartScan e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (ex_scan e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (...) {
					cerr << "Unnable to start the scan due to an unknow error. \n";
				}
			}
			// Start collecting data for specific scan.
			else if (strlen(cmd) > 6 && !strncmp(cmd, "start ", 6)) {
				// Get the id from the comand:
				string sCmd = cmd;

				// Find the space in the string
				size_t idIndex = sCmd.find(' ', 0);

				//Convert the string between the spaces into a number
				int id = atoi(sCmd.substr((idIndex + 1)).c_str());

				try {
					s3.StartScan(id);

					// Print legend.
					cout << setw(4) << "Sec" << setw(4) << "But" << setw(4) << 'X' << setw(4) << 'Y' << setw(4) << 'Z' << endl;
				}
				catch (ex_acq e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (ex_smartScan e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (ex_scan e) {
					cerr << e.what() << " Thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
				catch (...) {
					cerr << "Unnable to start the scan due to an unknow error. \n";
				}
			}
			// Stop collecting data.
			else if (!strcmp(cmd, "stop")) {
				s3.StopScan();
				cout << "All scans have stopped!" << string(70, ' ') << endl;
			}
			// Stop collecting data for a specific scan.
			else if (strlen(cmd) > 5 && !strncmp(cmd, "stop ", 5)) {
				// Get the id from the comand:
				string sCmd = cmd;
				// Find the space in the string
				size_t idIndex = sCmd.find(' ', 0);

				//Convert the string between the spaces into a number
				int id = atoi(sCmd.substr((idIndex + 1)).c_str());

				if( s3.StopScan(id) ) {
					cout << "Scan " << id << " has stopped!" << string(70, ' ') << endl;
				}
				else {
					cout << "Scan " << id << " has not been found!" << string(70, ' ') << endl;
				}
			}
			// Set the max amount of samples number, at which a scan would stop scanning
			else if (strlen(cmd) > 7 && !strncmp(cmd, "scan-size ", 10)) {
				// Get the id and size from the comand:
				string sCmd = cmd;

				// Find the space in the string
				size_t idIndex = sCmd.find(' ', 0);
				size_t sizeIndex = sCmd.find(' ', idIndex + 1);

				//Convert the string between the spaces into a number
				int id = atoi(sCmd.substr((idIndex + 1), (sizeIndex - idIndex + 1)).c_str());
				int size = atoi(sCmd.substr((sizeIndex + 1)).c_str());

				if (sCmd.c_str()[idIndex] == ' ' && sCmd.c_str()[sizeIndex] == ' ') {
					try {
						s3.SetStopSample(size, id);
						cout << "Max number of samples of scan " << id << " is set to: " << size << endl;
					}
					catch (ex_smartScan e) {
						cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
					}
				}
				else {
					cout << "Missing id and/or new sample size" << endl;
				}
			}
			// Delete all the scans.
			else if (!strcmp(cmd, "delete")) {
				char ack[128];
				cout << "Are you sure you want to delete all? answer y/n: ";
				cin.getline(ack, 128);

				if (!strncmp(ack, "y", 1)) {
					try {
						s3.DeleteScan();
						cout << "Deleted all scans!" << endl;
					}
					catch (ex_smartScan e) {
						cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
					}
				}
			}
			// Delete a specific scan.
			else if (strlen(cmd) > 7 && !strncmp(cmd, "delete ", 7)) {
				// Get the id from the comand:
				string sCmd = cmd;
				// Find the space in the string
				size_t idIndex = sCmd.find(' ', 0);

				//Convert the string between the spaces into a number
				int id = atoi(sCmd.substr((idIndex + 1)).c_str());

				try {
					s3.DeleteScan(id);
					cout << "Scan " << id << " has been deleted" << endl;
				}
				catch (ex_smartScan e) {
					cerr << e.what() << " thrown in function " << e.get_function() << " in file " << e.get_file() << endl;
				}
			}
			// Export x, y and z of specific scan.
			else if (strlen(cmd) > 7 && !strncmp(cmd, "export ", 7)) {
				// Get the id from the comand:
				string sCmd = cmd;
				sCmd.shrink_to_fit();

				size_t idIndex = sCmd.find(' ', 0);
				size_t nameIndex = sCmd.find(' ', idIndex + 1);
				int id = atoi(sCmd.substr((idIndex + 1), (nameIndex - idIndex + 1)).c_str());

				// Cut the filename out:
				string filename = sCmd.substr((nameIndex + 1), sCmd.max_size());
				cout << "exporting filtered data from scan: " << id << " into file: " << filename << endl;

				try {
					s3.ExportPointCloud(DIR_SCANS + filename + ".csv", id);
					cout << "Done.\n";
				}
				catch (ex_export e) {
					cerr << e.what() << endl;
				}
				catch (...) {
					cerr << "Could not export csv file" << endl;
				}
			}
			// Export all data of a specific scan.
			else if (strlen(cmd) > 12 && !strncmp(cmd, "full-export ", 12)) {
				// Get the id from the comand:
				string sCmd = cmd;
				sCmd.shrink_to_fit();

				size_t idIndex = sCmd.find(' ', 0);
				size_t nameIndex = sCmd.find(' ', idIndex + 1);
				int id = atoi(sCmd.substr((idIndex + 1), (nameIndex - idIndex + 1)).c_str());

				// Cut the filename out:
				string filename = sCmd.substr((nameIndex + 1), sCmd.max_size());
				cout << "exporting raw data from scan: " << id << " into file: " << filename << endl;

				try {
					s3.ExportCSV(DIR_SCANS + filename + ".csv", id);
					cout << "Done.\n";
				}
				catch (ex_export e) {
					cerr << e.what() << endl;
				}
				catch (...) {
					cerr << "Could not export csv file" << endl;
				}
			}
			// Export x,y,z raw data (except for the reference sensor) without any filtering, but with reference sensor correction.
			else if (strlen(cmd) > 11 && !strncmp(cmd, "export-raw ", 11)) {
				// Get the id from the comand:
				string sCmd = cmd;
				sCmd.shrink_to_fit();

				size_t nameIndex = sCmd.find(' ', 0);

				// Cut the filename out:
				string filename = sCmd.substr((nameIndex + 1), sCmd.max_size());
				cout << "Exporting raw data into file: " << filename << endl;

				try {
					s3.ExportPointCloud(DIR_SCANS + filename + ".csv", 0, true);
					cout << "Done.\n";
				}
				catch (ex_export e) {
					cerr << e.what() << endl;
				}
				catch (...) {
					cerr << "Could not export csv file" << endl;
				}
			}
			// Export all raw data (except for the reference sensor) without any filtering, but with reference sensor correction.
			else if (strlen(cmd) > 16 && !strncmp(cmd, "full-export-raw ", 16)) {
				// Get the id from the comand:
				string sCmd = cmd;
				sCmd.shrink_to_fit();

				size_t nameIndex = sCmd.find(' ', 0);

				// Cut the filename out:
				string filename = sCmd.substr((nameIndex + 1), sCmd.max_size());
				cout << "Exporting raw data into file: " << filename << endl;

				try {
					s3.ExportCSV(DIR_SCANS + filename + ".csv", 0, true);
					cout << "Done.\n";
				}
				catch (ex_export e) {
					cerr << e.what() << endl;
				}
				catch (...) {
					cerr << "Could not export csv file" << endl;
				}
			}
		}
		// Incase of an unknown command
		else {
			cout << "Command not recognized or unavailable, retry or type \"help\" to see all available commands." << endl;
		}
	} while (strcmp(cmd, "exit"));	// Exit the application.
}

// Help menu.
void Usage()
{
	cout << endl;
	cout << "\tCommand" << "\t\t\t\t" << "Description" << endl;
	cout << "\t*start [id]\t\t\tStart the measurement. Leave id blank to start all scans." << endl;
	cout << "\t*stop [id]\t\t\tStop the measurement. Leave id blank to stop all scans." << endl;
	cout << "\tnew\t\t\t\tCreate a new measurement." << endl;
	cout << "\tcalibrate\t\t\tCalibrate the sensor offsets with respect to finger t h i c c n e s s." << endl;
	cout << "\t*scan-size [id] [size]\t\tSet the max amount of samples to be measured." << endl << "\t\t\t\t\tScanning will stop at this number of samples (-1 for infite duration)." << endl;
	cout << "\tclear\t\t\t\tClear all recorded data." << endl;
	cout << "\t*delete [id]\t\t\tDelete a measurement. Leave id blank to delete all scans" << endl << "\t\t\t\t\t" << "and clear the raw data." << endl;
	cout << "\tlist\t\t\t\tPrint all the existing Scans to the console." << endl;
	cout << "\t*export [id] [filename]\t\tExport the filtered data of the scan id as a CSV file with" << endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << endl << "\t\t\t\t\tIncludes: x, y, z." << endl;
	cout << "\t*full-export [id] [filename]\tExport the filtered data of the scan id as a CSV file with" << endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << endl << "\t\t\t\t\tIncludes: time, x, y, z, roll, elevation, azimuth, quality" << endl << "\t\t\t\t\t\t  and button state." << endl;
	cout << "\t*export-raw [filename]\t\tExport the raw data of all the sensors as a CSV file with" << endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << endl << "\t\t\t\t\tIncludes: x, y, z." << endl;
	cout << "\t*full-export-raw [filename]\tExport the raw data of all the sensors as a CSV file with" << endl << "\t\t\t\t\tthe given filename (no spaces allowed in filename)." << endl << "\t\t\t\t\tIncludes: time, x, y, z, roll, elevation, azimuth, quality " << endl << "\t\t\t\t\t\t  and button state." << endl;
	cout << "\thelp \t\t\t\tPrint this screen again." << endl;
	cout << "\texit \t\t\t\tCleanly exit the application." << endl;
	cout << endl;
	cout << "\t* \t\t\t\tCommand is only allowed to be run when at least 1 scan is created with \"new\"" << endl;
	cout << endl;
}

// Function that is called everytime a new set of samples has been collected.
void RawPrintCallback(const vector<Point3>& record)
{
	cout << '\r'; // Set cursor to the beginning of the line to prevent messy couts.
	// All data is casted to an int so that the amount of characters that are going to be printed are predictable and concise.
	// This is important since you can only rewrite over the current line, so all sensor data needs to be printed on one line.
	cout << setw(4) << (int)record[0].time;
	cout << setw(4) << (int)record[0].buttonState;

	for (int i = 0; i < record.size(); i++) {
		cout << setw(5) << (int)record[i].x;
		cout << setw(5) << (int)record[i].y;
		cout << setw(5) << (int)record[i].z;
	}
	cout << ' ' << '\r' << flush;
}

// Identify a specific sensor serial ID
int SensorIdentification(const char* sensorParam) {
	char input[128];

	// Open and/or make INI file in case it has not been made
	if (!fs::exists(DIR_SETTINGS)) {
		// Create settings INI
		fstream settingsINI(DIR_SETTINGS);
		settingsINI.open(DIR_SETTINGS, ios::in | ios::out | ios::trunc);
		settingsINI.close();
	}

	// Add sensor section incase it's not present
	if (!iniFile.has_section(SECTION_SENSOR)) {
		iniFile.add_section(SECTION_SENSOR);
		iniFile.write(DIR_SETTINGS);
	}

	if (!iniFile[SECTION_SENSOR].has_key(sensorParam) || !s3.IsSerialConnected(iniFile[SECTION_SENSOR].get<int>(sensorParam))) {
		cout << "\t\tIs there a " << sensorParam << " sensor (y/n) > " << flush;
		do {
			cin.getline(input, 128);

			if (strncmp(input, "y", 1) == 0 || strncmp(input, "Y", 1) == 0) {
				// Check which sensor is the highest
				cout << "\t\t\tPlease hold up the " << sensorParam << " sensor and hit enter when ready." << flush;
				cin.ignore(10, '\n');
				iniFile[SECTION_SENSOR].set<int>(sensorParam, s3.HighestSensor());
				iniFile.write(DIR_SETTINGS);
				cout << "\t\t\t\t" << sensorParam << " set to: " << iniFile[SECTION_SENSOR].get<int>(sensorParam) << endl;
				return iniFile[SECTION_SENSOR].get<int>(sensorParam);
			}
			else if (strncmp(input, "n", 1) == 0 || strncmp(input, "N", 1) == 0) {
				cout << "\t\t\t\tSkipped: " << sensorParam << " sensor." << endl; 
				iniFile[SECTION_SENSOR].set<int>(sensorParam, -1);
				iniFile.write(DIR_SETTINGS);
				return iniFile[SECTION_SENSOR].get<int>(sensorParam);
			}
			else {
				cout << "\t\t\tIncorrect input please try again." << flush;
			}
		} while (true);
	}
	else {
		return iniFile[SECTION_SENSOR].get<int>(sensorParam);
	}
}

int GetSensorID(const char* parameter) {
	if (iniFile[SECTION_SENSOR].has_key(parameter)) {
		return iniFile[SECTION_SENSOR].get<int>(parameter);
	}
	else {
		return -1;
	}

}