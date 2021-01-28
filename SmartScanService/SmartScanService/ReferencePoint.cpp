#include "ReferencePoint.h"

using namespace SmartScan;

ReferencePoint::ReferencePoint() :index{ 0 },pos{Point3()}{}

ReferencePoint::ReferencePoint(int index, double x, double y, double z)
{
	this->index = index;
	this->pos = Point3(x, y, z);
}
ReferencePoint::ReferencePoint(int index, Point3 point)
{
	this->index = index;
	this->pos = point;
}