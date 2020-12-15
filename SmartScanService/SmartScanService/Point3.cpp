#include "Point3.h"

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
Point3::Point3(double x, double y, double z, double rx, double ry, double rz)
{
	this->x = x;
	this->y = y;
	this->z = z;

	Rotation3 r(rx, ry, rz);
	this->r = r;
}

