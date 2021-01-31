#pragma once


#include "Point3.h";

namespace SmartScan {
	class ReferencePoint
	{
	public:
		int index;
		Point3 pos;
		Point3 refSensorPos;

		ReferencePoint(int index, double x, double y, double z);
		ReferencePoint(int index, Point3 point, Point3 refSensorPos);
		ReferencePoint(int index, Point3 point);
		ReferencePoint();
	private:

	};
}