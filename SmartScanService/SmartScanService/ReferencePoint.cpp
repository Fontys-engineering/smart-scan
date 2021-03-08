#include "ReferencePoint.h"

using namespace SmartScan;

ReferencePoint::ReferencePoint() :index{ 0 },pos{Point3()},refSensorPos{Point3()}
{

}

ReferencePoint::ReferencePoint(int index, double x, double y, double z)
{
	this->index = index;
	this->pos = Point3(x, y, z);
}

SmartScan::ReferencePoint::ReferencePoint(int index, Point3 point, Point3 refSensorPos): index{index}, pos{point}, refSensorPos{refSensorPos}
{

}

ReferencePoint::ReferencePoint(int index, Point3 point):index{index}, pos{point}, refSensorPos{Point3()}
{

}