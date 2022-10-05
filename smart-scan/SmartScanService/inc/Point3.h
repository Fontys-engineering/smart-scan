// These are the SmartScan point classes.
// They contain all the information a TrakStar sample provides in one object.

#pragma once

#include <iostream>
#include <float.h>

namespace SmartScan
{
    // Enum containing all the possible button classifications.
    enum class button_state {
        INVALID,                            // Error point classification. (is not really used).
        BAD,                                // Not on the surface of the foot classification.
        REFERENCE,                          // On the surface of the foot classification.
        MANIPULATE,                         // Manipulation by the clinician classification.
    };

    // Class containing the euler angles of the sensor.
    class Rotation3 {
    public:
        double x, y, z;                     // X, Y and Z rotations (Roll, Elevation and Azimuth).

		// Constructor. Creates a Rotation3 object that contains the euler angle rotations.
		// Arguments:
        // - x : Rotation around the X axis (Roll).
        // - y : Rotation around the Y axis (Elevation).
        // - z : Rotation around the Z axis (Azmimuth).
        Rotation3();
        Rotation3(double x, double y, double z);
    };

    // Class containing the sensor position in spherical notation.
    class Spherical3 {
    public:
        double r, phi, theta;               // Radius, phi and theta.

		// Constructor. Creates a Spherical3 object that contains the position in spherical notation.
		// Arguments:
        // - r : Radius (Length of the vector).
        // - theta : Theta (Rotation around the Z axis).
        // - phi : Phi (Elevation of the vector, angle between vector and X,Y plane).
        Spherical3();
        Spherical3(double r, double theta, double phi);
    };

    // Class containing all the needed information for a sample.
    class Point3 {
    public:
        double x, y, z, time;               // X, Y, Z and time.
        unsigned short quality;             // Indicates magnetic interference.
        unsigned short button;              // Indicates a button press.

        Rotation3 r;                        // Roation object.
        Spherical3 s;                       // Spherical object.

        button_state buttonState;           // Button_state enum.

		// Constructor. Creates a Point3 object that contains the needed information for a sample.
        // Arguments:
        // - x : X position of the sensor.
        // - y : Y position of the sensor.
        // - z : Z position of the sensor.
        // - r : Rotation3 object.
        // - rx : Rotation around the X axis (Roll).
        // - ry : Rotation around the Y axis (Elevation).
        // - rz : Rotation around the Z axis (Azimuth).
        // - s : Spherical3 object.
        // - sr : Radius (Length of the vector).
        // - stheta : Radius (Rotation around the Z axis).
        // - sphi : Radius (Elevation of the vector, angle between vector and X,Y plane).
        // - q : Quality, indicates magnetic interference.
        // - button : Button, indicates a button press.
        Point3();
        Point3(double x, double y, double z);
        Point3(double x, double y, double z, Rotation3 r);
        Point3(double x, double y, double z, Spherical3 s);
        Point3(double x, double y, double z, Rotation3 r, Spherical3 s);
        Point3(double x, double y, double z, double rx, double ry, double rz);
        Point3(double x, double y, double z, double rx, double ry, double rz, unsigned short q, unsigned short button);
        Point3(double x, double y, double z, double rx, double ry, double rz, double sr, double sphi, double stheta);
    };

    // Class similar to Point3 but only contains the information needed for the reference sensor.
    class Point3Ref {
    public:
        double x, y, z;                         // X, Y and Z coordinates.
        double m[3][3];                         // Rotation matrix.

		// Constructor. Creates a Point3Ref object that contains the needed information for the reference sensor.
        // Arguments:
        // - x : X position of the sensor.
        // - y : Y position of the sensor.
        // - z : Z position of the sensor.
        // - m : 3x3 array containing the Rotation matrix. 
        Point3Ref();
        Point3Ref(double x, double y, double z);
        Point3Ref(double x, double y, double z, double m[3][3]);
    };
}