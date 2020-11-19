#pragma once
//This class describes the main data format used throughout the SmartScan software.
//It helps contain one TrakSTAR measurement data point (position, location) in one object.

namespace SmartScan
{
    class Rotation3
    {
    public:
        double x, y, z;

        Rotation3();
        Rotation3(double x, double y, double z);
    };
    class Point3
    {
    public:
        double x, y, z;
        Rotation3 r;

        Point3();
        Point3(double x, double y, double z);
        Point3(double x, double y, double z, Rotation3 r);
        Point3(double x, double y, double z, double rx, double ry, double rz);

    };
}