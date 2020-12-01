
// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This provides an interface for the SmartScanServices.

//include all the libraries and sub-classes:
#include "SmartScanCLI.h"

using namespace SmartScan;


int main()
{
    std::cout << "Smart Scan Command Line Interface Application" << std::endl;

    //create a new SmartScanService object:
    //SmartScanService s3;

    //initialise the service:
    try {
        //s3.Init();
    }
    catch (...)
    {
        std::cerr << "ERROR: Could not initialise Smart Scan Service" << std::endl;
    }
    //print the help screen:
    Usage();

    char cmd[128];
    do {
        std::cin >> cmd;
        if (!strcmp(cmd,"start"))
        {
            //s3.StartScan();
        }
        else
        {
            Usage();
        }
    } while (!strcmp(cmd, "exit"));

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
    std::cout << "\t progress \t\t Get an estimate of the scan's completion" << std::endl;
    std::cout << "\t export [filename] \t Export the processed data as a CSV file with the given filename" << std::endl;
    std::cout << std::endl;
    std::cout << "System preferences" << std::endl;
    std::cout << "\t calibrate \t\t Begin the glove calibration process" << std::endl;
    std::cout << std::endl;
    std::cout << "CLI usage" << std::endl;
    std::cout << "\t help \t\t\t print this screen again" << std::endl;
    std::cout << "\t exit \t\t\t cleanly exit the application" << std::endl;


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
