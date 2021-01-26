#include "Filtering.h"

using namespace SmartScan;

// Filtering helper methods:

SmartScan::Filtering::Filtering()
{

}

// RotationOrientation calculates new x, y and z values based on the azimuth, elevation and roll values.
// At this point, the input 'data' only consists of x-y-z values and azimuth, elevation and roll values
// The output 'data' consists of new x-y-z values with the according azimuth, elevation and roll values.
std::vector<Point3>& SmartScan::Filtering::RotationOrientation(std::vector<Point3>& data)
{
    /* All points will be rotated to the fixed values given.This point's
    orientation will be taken as a reference to transform all other
    points back to. */
    double azimuth_ref = 0;
    double elevation_ref = 0;
    double roll_ref = 0;

    // Declare the output data set
    std::vector<Point3> outputData;

    // Recoordinate every point to the first point's orientation
    for (int i = 0; i < data.size(); i++)
    {
        // Declare new variables for new points
        double x_new = 0;
        double y_new = 0;
        double z_new = 0;

        // Check the orientation of the current point
        double x = data[i].x;
        double y = data[i].y;
        double z = data[i].z;
        double azimuth = data[i].r.z;
        double elevation = data[i].r.y;
        double roll = data[i].r.x;

        // Calculate the difference in azimuth between the current point and the reference point
        double azimuth_diff = azimuth - azimuth_ref;
        double elevation_diff = elevation - elevation_ref;
        double roll_diff = roll - roll_ref;

        // This difference can be used to change the phi angle of the
        // current point, after this we can calculate the new XYZ values
        // with the r, phiand theta values.We assume the Azimuth only
        // affects the XY planeand Elevation the XZ plane, while the
        // rotation affects the YZ plane

        // Use the azimuth to calculate the rotation around the z-axis
        double azimuth_distance = sqrt(pow(x, 2) + pow(y, 2));
        double a = arctan(x, y) - azimuth_diff;
        x_new = azimuth_distance * cos(a);
        y_new = azimuth_distance * sin(a);
      
        // Use the elevation to calculate the rotation around the y-axis
        double elevation_distance = sqrt(pow(x_new, 2) + pow(z, 2));
        double b = arctan(x_new, z) + elevation_diff;
        x_new = elevation_distance * cos(b);
        y_new = elevation_distance * sin(b);

        // Use the roll difference to calculate the rotation around the x-axis
        double roll_distance = sqrt(pow(y_new, 2) + pow(z_new, 2));
        double c = arctan(y_new, z_new) - roll_diff;
        y_new = roll_distance * cos(c);
        z_new = roll_distance * sin(c);
        
        outputData.push_back(Point3(x_new, y_new, z_new, roll_ref, elevation_ref, azimuth_ref, 0, 0, 0));
    }
    return outputData;
}

double SmartScan::Filtering::arctan(double a, double b)
{
    double pi = 3.14159265;
    double result;

    if (a == 0 && b == 0)
        result = 0;
    else if (a >= 0 && b > 0)
        result = atan2(b, a)* 180/pi;
    else if (a < 0 && b >= 0)
        result = (atan2(b, a)*(180/pi)) + 180;
    else if (a <= 0 && b < 0)
        result = (atan2(b, a)*(180/pi)) - 180;
    else if (a > 0 && b <= 0)
        result = atan2(b, a)*180/pi;
    return result;
}




