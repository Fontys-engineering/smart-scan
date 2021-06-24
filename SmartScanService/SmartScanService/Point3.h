#pragma once

#include <float.h>

namespace SmartScan
{
    enum class button_state
    {
        INVALID,
        BAD,
        REFERENCE,
        MANIPULATE,  
    };

    class Rotation3
    {
    public:
        double x, y, z;

        Rotation3();
        Rotation3(double x, double y, double z);
    };

    class Spherical3
    {
    public:
        double r, phi, theta;

        Spherical3();
        Spherical3(double r, double phi, double theta);
    };

    // This class describes the main data format used throughout the SmartScan software.
    // It helps contain one TrakSTAR measurement data point (position, location and time) in one object.
    class Point3
    {
    public:
        double x, y, z, time;
        unsigned short quality;
        unsigned short button;

        Rotation3 r;
        Spherical3 s;

        button_state buttonState;

        Point3();
        Point3(double x, double y, double z);
        Point3(double x, double y, double z, Rotation3 r);
        Point3(double x, double y, double z, Spherical3 s);
        Point3(double x, double y, double z, Rotation3 r, Spherical3 s);
        Point3(double x, double y, double z, double rx, double ry, double rz);
        Point3(double x, double y, double z, double rx, double ry, double rz, unsigned short q, unsigned short button);
        Point3(double x, double y, double z, double rx, double ry, double rz, double sr, double sphi, double stheta);
    };

    class Point3Ref
    {
    public:
        double x, y, z;
        double m[3][3];

        Point3Ref();
        Point3Ref(double x, double y, double z);
        Point3Ref(double x, double y, double z, double m[3][3]);
    };
}