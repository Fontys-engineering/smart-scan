
// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This provides an interface for the SmartScanServices.

//include all the libraries and sub-classes:
#include "SmartScanCLI.h"
#include "Exceptions.h"

using namespace SmartScan;


int main()
{
    std::cout << "Smart Scan Command Line Interface Application" << std::endl;

    //create a new SmartScanService object with mock data:
    SmartScanService s3(true);

    //initialise the service:
    try {
        std::cout << "Initialising the TrakSTAR device. This can take up to a minute. \n";
        s3.Init();
        std::cout << "TrakSTAR device initialisation done. \n";

        //register the callback (uncomment this to see a demo. Warninig: it will flood the console with values):
        //s3.RegisterNewDataCallback(TestUICallback);
    }
    catch (ex_trakStar &e)
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
        std::cout<<std::endl << "SmartScan>";
        std::cin.getline(cmd, 128);
         if (!strcmp(cmd,"start"))
        {
            try
            {
                s3.StartScan();
            }
            catch(std::exception e)
            {
                std::cerr << e.what() << std::endl;
            }
            catch (ex_trakStar e)
            {
                std::cerr << e.what() << "thrown in function "<< e.get_function()<< " in file "<<e.get_file()<< std::endl;
            }
            catch (ex_scan e)
            {
                std::cerr << e.what() << "thrown in function " << e.get_function() << " in file " << e.get_file() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Unnable to start the scan \n";
            }
        }
        else if (!strcmp(cmd, "stop"))
        {
            s3.StopScan();
        }
        else if (!strcmp(cmd, "dump"))
        {
            s3.DumpScan();
        }
        else if (strlen(cmd) > 7 && !strncmp(cmd, "export " , 7))
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
    std::cout << "Once the service is running, you can controll the system by typing the following commands:" << std::endl << std::endl;
    std::cout << "Measurement control" << std::endl;
    std::cout << "\t start \t\t\t Start a new measurement" << std::endl;
    std::cout << "\t stop \t\t\t Stop the current measurement" << std::endl;
    std::cout << "\t dump \t\t\t Print all the records to the console (for debugging)" << std::endl;
    std::cout << "\t progress \t\t Get an estimate of the scan's completion" << std::endl;
    std::cout << "\t export [filename] \t Export the processed data as a CSV file with the given filename (no spaces allowed in the filename)" << std::endl;
    std::cout << std::endl;
    std::cout << "System preferences" << std::endl;
    std::cout << "\t calibrate \t\t Begin the glove calibration process" << std::endl;
    std::cout << std::endl;
    std::cout << "CLI usage" << std::endl;
    std::cout << "\t help \t\t\t print this screen again" << std::endl;
    std::cout << "\t exit \t\t\t cleanly exit the application" << std::endl;


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
