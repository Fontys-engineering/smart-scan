#include "Point3.h"
#include <iostream>
using namespace SmartScan;

Rotation3::Rotation3()
{
	this->x = this->y = this->z = 0;
}

Rotation3::Rotation3(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Spherical3::Spherical3()
{
	this->r = this->phi = this->theta = 0;
}

Spherical3::Spherical3(double r, double phi, double theta)
{
	this->r = r;
	this->phi = phi;
	this->theta = theta;
}

Point3::Point3()
{
	this->x = this->y = this->z = 0;
}

Point3::Point3(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point3::Point3(double x, double y, double z, Rotation3 r)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->r = r;
}

Point3::Point3(double x, double y, double z, Spherical3 s)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
}

Point3::Point3(double x, double y, double z, Rotation3 r, Spherical3 s)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->r = r;
	this->s = s;
}

Point3::Point3(double x, double y, double z, double rx, double ry, double rz)
{
	this->x = x;
	this->y = y;
	this->z = z;

	Rotation3 r(rx, ry, rz);
	this->r = r;
}

Point3::Point3(double x, double y, double z, double rx, double ry, double rz, double sr, double sphi, double stheta)
{
	this->x = x;
	this->y = y;
	this->z = z;

	Rotation3 r(rx, ry, rz);
	this->r = r;
	Spherical3 s(sr, sphi, stheta);
	this->s = s;
}
Point3::Point3(double x, double y, double z, double rx, double ry, double rz, unsigned short q, unsigned short button)
	: x{ x }, y{ y }, z{ z }, r{ Rotation3(rx,ry,rz) }, quality{ q }, button{ button }
{
	
}


Point3Ref::Point3Ref()
{

}

Point3Ref::Point3Ref(double x, double y, double z) : x { x }, y { y }, z { z }
{

}

Point3Ref::Point3Ref(double x, double y, double z, double m[3][3]) : x { x }, y { y }, z { z }
{
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            this->m[i][k] = m[i][k];
        }
    }
}

void Point3Ref::inverseRotm()
{
	double temp[3][3];

	double detDownRig = m[1][1]*m[2][2] - m[2][1]*m[1][2];
	double detDownMid = m[2][0]*m[1][2] - m[1][0]*m[2][2];
	double detDownLef = m[1][0]*m[2][1] - m[2][0]*m[1][1];

	double detAll = m[0][0]*detDownRig + m[0][1]*detDownMid + m[0][2]*detDownLef;
	double invDet = 1.0/detAll;

	temp[0][0] = detDownRig * invDet;
	temp[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
	temp[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
	temp[1][0] = detDownMid * invDet;
	temp[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
	temp[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet;
	temp[2][0] = detDownLef * invDet;
	temp[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet;
	temp[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;

	for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            this->m[i][k] = temp[i][k];
        }
    }
}