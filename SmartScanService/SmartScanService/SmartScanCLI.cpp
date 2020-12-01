// SmartScanService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This provides an interface for the SmartScanServices.

//include all the libraries and sub-classes:
#include "SmartScanCLI.h"
#include "TrakStarController.h"

using namespace SmartScan;

int main()
{
    std::cout << "Smart Scan!\n";

    //example Point3:
    double x = 1;
    double y = 2;
    double z = 3;
    Point3 exampleDataPoint(x, y, z);

    std::cout << "Point3 Example: x=" << exampleDataPoint.x << " y=" << exampleDataPoint.y << " z=" << exampleDataPoint.z << std::endl;

    TrakStarController tSCtrl;
    tSCtrl.Init();
    tSCtrl.Config();
    tSCtrl.AttachSensor();
    tSCtrl.ReadSensor();
    tSCtrl.StopTransmit();
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
