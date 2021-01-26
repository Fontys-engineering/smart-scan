#include "Filtering.h"

using namespace SmartScan;

//Filtering helper methods:
void RotationOrientation(std::vector<Point3>& data, std::vector<Point3>& referenceData);

void FilterIteration(std::vector<Point3>& data, std::vector<ReferencePoint> referencePoints, double resolution);

void CalculateCoordinates(ReferencePoint ref, std::vector<Point3>& data);

void Outlier(std::vector<Point3>& data, double phi_range, double theta_range);

/* <summary
* Calculate the arc tangent of two points. Output = arctan(b/a)
* Takes into account the four quadrants of the function, so range is from -180 to 180 degrees
* </summary>
*/
double arctan(double a, double b);

bool TestPoint(std::vector<Point3>& data, double phi_range, double theta_range);

std::vector<Point3>& GradientSmoothing(std::vector<Point3>& data, double phi_range, double theta_range);

std::vector<std::vector<Point3>>& SortArrays(std::vector<Point3>& data, std::vector<Point3> reference_data);