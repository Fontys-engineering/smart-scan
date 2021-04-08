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