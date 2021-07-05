#include "Point3.h"

using namespace SmartScan;

Rotation3::Rotation3()
{
	// Initialize x, y and z rotations to be 0.
	this->x = this->y = this->z = 0;
}

Rotation3::Rotation3(double x, double y, double z)
	: x { x }, y { y }, z { z }
{

}

Spherical3::Spherical3()
{
	// Initialze radius to be the max value it can be and phi and theta to be 0.
	this->r = DBL_MAX;
	this->theta = this->phi = 0;
}

Spherical3::Spherical3(double r, double theta, double phi)
	: r { r }, theta { theta }, phi { phi }
{

}

Point3::Point3()
{
	// Initialize x, y, z, quality and button to be 0.
	this->time = this->x = this->y = this->z = this->quality = this->button = 0;
}

Point3::Point3(double x, double y, double z)
	: x { x }, y { y }, z { z }
{

}

Point3::Point3(double x, double y, double z, Rotation3 r)
	: x { x }, y { y }, z { z }, r { r }
{

}

Point3::Point3(double x, double y, double z, Spherical3 s)
	: x { x }, y { y }, z { z }, s { s }
{

}

Point3::Point3(double x, double y, double z, Rotation3 r, Spherical3 s) 
	: x { x }, y { y }, z { z }, r { r }, s { s }
{

}

Point3::Point3(double x, double y, double z, double rx, double ry, double rz)
	: x { x }, y { y }, z { z }, r { Rotation3(rx, ry, rz) }
{

}

Point3::Point3(double x, double y, double z, double rx, double ry, double rz, double sr, double stheta, double sphi)
	: x { x }, y { y }, z { z }, r { Rotation3(rx, ry, rz) }, s { Spherical3(sr, stheta, sphi) }
{

}

Point3::Point3(double x, double y, double z, double rx, double ry, double rz, unsigned short q, unsigned short button)
	: x{ x }, y{ y }, z{ z }, r{ Rotation3(rx, ry, rz) }, quality{ q }, button{ button }
{
	
}

Point3Ref::Point3Ref()
{
	// Initialize x, y and z to be 0.
	this->x = this->y = this->z = 0;

	// Initialize rotation matrix with no rotation.
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
			if (k == i) {
				this->m[i][k] = 1;
			}
			else {
				this->m[i][k] = 0;
			}
        }
	}
}

Point3Ref::Point3Ref(double x, double y, double z) : x { x }, y { y }, z { z }
{
	// Initialize rotation matrix with no rotation.
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
			if (k == i) {
				this->m[i][k] = 1;
			}
			else {
				this->m[i][k] = 0;
			}
        }
	}
}

Point3Ref::Point3Ref(double x, double y, double z, double m[3][3]) : x { x }, y { y }, z { z }
{
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            this->m[i][k] = m[k][i]; // Transpose matrix.
        }
    }
}