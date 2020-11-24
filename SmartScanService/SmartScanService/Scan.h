#pragma once

#include "Point3.h"
#include "ReferencePoint.h"

#include <vector>
namespace SmartScan
{
	class Scan
	{
	public:
		std::vector<Point3> inBuff;
		std::vector<Point3> outBuff;

		void StartScan();
		void StopScan(bool clearData = false);
		double GetScanCompletion();
		bool ScanComplete();
		void PostProcessScan();
	private:

	};
}