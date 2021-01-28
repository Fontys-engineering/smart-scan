#include "Filtering.h"

using namespace SmartScan;

// Filtering helper methods:

SmartScan::Filtering::Filtering()
{

}
SmartScan::Filtering::Filtering(std::vector<ReferencePoint> ref_point, double phi_range, double theta_range) : referencePoints{ ref_point }, phi_range{ phi_range }, theta_range{ theta_range }{}

// RotationOrientation calculates new x, y and z values based on the azimuth, elevation and roll values.
// At this point, the input 'data' only consists of x-y-z values and azimuth, elevation and roll values
// The output 'data' consists of new x-y-z values with the according azimuth, elevation and roll values.
void SmartScan::Filtering::RotationOrientation(std::vector<Point3>& data, std::vector<Point3>& referenceData)
{
    /* All points will be rotated to the fixed values given.This point's
    orientation will be taken as a reference to transform all other
    points back to. */
    double azimuth_ref = 0;
    double elevation_ref = 0;
    double roll_ref = 0;
    
    // Declare the output data set
    std::vector<Point3> outputData;
    unsigned int nOfSensors = data.size() / referenceData.size();
    // Recoordinate every point to the first point's orientation
    for (unsigned int i = 0; i < referenceData.size(); i++)
    {
        for (unsigned int j = 0; j < nOfSensors; j++)
        {
            const double pi = 3.14159265;
            // Declare new variables for new points
            double x_new = 0;
            double y_new = 0;
            double z_new = 0;

            // Check the orientation of the current point
            const double x = data[(i*nOfSensors)+j].x - referenceData[i].x;
            const double y = data[(i * nOfSensors) + j].y - referenceData[i].y;
            const double z = data[(i * nOfSensors) + j].z - referenceData[i].z;
            const double azimuth = referenceData[i].r.z;
            const double elevation = referenceData[i].r.y;
            const double roll = referenceData[i].r.x;

            // Calculate the difference in azimuth between the current point and the reference point
            const double azimuth_diff = azimuth - azimuth_ref;
            const double elevation_diff = elevation - elevation_ref;
            const double roll_diff = roll - roll_ref;

            // This difference can be used to change the phi angle of the
            // current point, after this we can calculate the new XYZ values
            // with the r, phiand theta values.We assume the Azimuth only
            // affects the XY planeand Elevation the XZ plane, while the
            // rotation affects the YZ plane

            // Use the azimuth to calculate the rotation around the z-axis
            const double azimuth_distance = sqrt(pow(x, 2) + pow(y, 2));
            const double a = (atan2(y, x) * 180 / pi) - azimuth_diff;
            x_new = azimuth_distance * cos(a * pi/180);
            y_new = azimuth_distance * sin(a * pi/180);

            // Use the elevation to calculate the rotation around the y-axis
            const double elevation_distance = sqrt(pow(x_new, 2) + pow(z, 2));
            const double b = (atan2(z, x_new) * 180 / pi) + elevation_diff;
            x_new = elevation_distance * cos(b * pi / 180);
            z_new = elevation_distance * sin(b * pi / 180);

            // Use the roll difference to calculate the rotation around the x-axis
            const double roll_distance = sqrt(pow(y_new, 2) + pow(z_new, 2));
            const double c = (atan2(z_new, y_new) * 180 / pi) - roll_diff;
            y_new = roll_distance * cos(c * pi / 180);
            z_new = roll_distance * sin(c * pi / 180);

            outputData.push_back(Point3(x_new, y_new, z_new, roll_ref, elevation_ref, azimuth_ref, 0, 0, 0));
            
        }
    }
    data = outputData;
}

void SmartScan::Filtering::Outlier(std::vector<Point3>& data, double phi_range, double theta_range)
{
    for (int l = -180 + phi_range * 2; l < 180 - phi_range * 2; l += phi_range * 4) 
    {
        for (int m = -180 + theta_range * 2; m < theta_range * 4; m += 180 - theta_range * 2) 
        {
            int locationArray[] = { 0 };
            int tempIndex = 1;
            for (int k = 0; k < data.size(); k++) 
            {
                if ((data[k].s.phi <= l + phi_range * 2) && (data[k].s.phi >= l - phi_range * 2)) 
                {
                    if ((data[k].s.theta <= m + theta_range * 2) && (data[k].s.theta >= m - theta_range * 2)) 
                    {
                        locationArray[tempIndex] = k;
                        tempIndex = tempIndex++;
                    }
                }
            }

            if (locationArray[1] > 0)
            {
                double checkArray[] = { 0 };
                const int szloc = sizeof(locationArray);
                for (int ind = 0; ind < szloc; ind++) 
                {
                    checkArray[ind] = data[locationArray[ind]].s.r;
                }
                if (szloc > 2)
                {
                    double mean = 0;
                    mean = findMean(checkArray, sizeof(checkArray));
                    int tf[szloc] = { 0 };
                    for (int i = 0; i < szloc; i++)
                    {
                        if (checkArray[i] > mean * 1.2)
                        {
                            tf[i] = 1;
                        }
                    }
                    int a[] = { 0 };
                    int TI = 0;
                    for (int j = 0; j < szloc; j++)
                    {
                        if (tf[j] > 0)
                        {
                            a[TI] = j;
                            TI++;
                        }
                    }
                
                    int sza = sizeof(a);
                    if (sza > 0) 
                    {
                        int deletedItems = 0;
                        for (int inda = 0; inda < sza; inda++) 
                        {
                            int indexToRemove = locationArray[a[inda]] - deletedItems;

                            data.erase(data.begin() + indexToRemove);
                            deletedItems++;
                        }
                    }
                }
            }
        }
    }
}


void SmartScan::Filtering::FilterIteration(std::vector<Point3>& data, std::vector<ReferencePoint>& referencePoints, double phi_range, double theta_range)
{
    std::vector<std::vector<Point3>> vectorSet;
    std::vector<std::vector<Point3>> vectorSetSort;
    std::vector<Point3> f_data;
    Filtering f;
    vectorSet = CalculateCoordinates(referencePoints, data);
    vectorSetSort = SortArrays(data, vectorSet, referencePoints);
    
    for (int i = 0; i < referencePoints.size(); i++)
    {
        GradientSmoothing(vectorSetSort[i], phi_range, theta_range);
        Outlier(vectorSetSort[i], phi_range, theta_range);
        for (auto j = 0; j < vectorSetSort[i].size(); j++)
        {
            f_data.emplace_back(vectorSetSort[i][j]);
        }
    }
    data = f_data;
}


void SmartScan::Filtering::Filter(std::vector<Point3>& data, std::vector<Point3>& referenceData)
{
    RotationOrientation(data, referenceData);
    FilterIteration(data, this->referencePoints, this->phi_range, this->theta_range);
}

void SmartScan::Filtering::SetReferencePoints(std::vector<ReferencePoint> referencePoints)
{
    this->referencePoints = referencePoints;

}

void SmartScan::Filtering::SetResolution(double phi_range, double theta_range)
{
    this->phi_range = phi_range;
    this->theta_range = theta_range;
}

std::vector<std::vector<Point3>> SmartScan::Filtering::CalculateCoordinates(std::vector<ReferencePoint>& ref, std::vector<Point3>& data)
{
    std::vector<std::vector<Point3>> vectorSet;
    double pi = 3.1415;

    for (int i = 0; i < ref.size(); i++)
    {
        vectorSet.emplace_back(std::vector<Point3>());
        for (int count = 0; count < data.size(); count++)
        {
            vectorSet[i].emplace_back(Point3());
            vectorSet[i][count].s.r = sqrt(pow(data[count].x - ref[i].pos.x, 2) + pow(data[count].y - ref[i].pos.y, 2) + pow(data[count].z - ref[i].pos.z, 2));
            vectorSet[i][count].s.phi = atan2(data[count].y - ref[i].pos.y ,data[count].x - ref[i].pos.x) * 180/pi;
            vectorSet[i][count].s.theta = acos((data[count].z - ref[i].pos.z) / sqrt(pow(data[count].x - ref[i].pos.x, 2) + pow(data[count].y - ref[i].pos.y, 2) + pow(data[count].z - ref[i].pos.z, 2))) * 180/pi;
        }
	}

    return vectorSet;
}

bool SmartScan::Filtering::TestPoint(std::vector<Point3>& data, double phi_range, double theta_range, int index)
{
	bool result = true;

	// Filter nearest points(true) from outliers(false)
	for (int j = 0; j< data.size(); j++) 
	{
		if (index < data.size())
		{
			if ((data[j].s.phi <= data[index].s.phi + phi_range / 2) && (data[j].s.phi >= data[index].s.phi - phi_range / 2)) 

			{
				if ((data[j].s.phi <= data[index].s.phi + phi_range / 2) && (data[j].s.phi >= data[index].s.phi - phi_range / 2))
				{
					if ((data[j].s.theta <= data[index].s.theta + theta_range / 2) && (data[j].s.theta >= data[index].s.theta - theta_range / 2))
					{
						if (data[j].s.r < data[index].s.r)
						{
							result = false;
						}
					}
				}
			}
		}
	}
	return result;
}

void SmartScan::Filtering::GradientSmoothing(std::vector<Point3>& data, double phi_range, double theta_range)
{
    int index = 0;

    while (index <= data.size())
    {
        if (index < data.size())
        {
            if (TestPoint(data, phi_range, theta_range, index) == false)
            {
                data.erase(data.begin() + index);
                index--;
            }
        }
        index++;
    }
}

double SmartScan::Filtering::findMean(double a[], int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += a[i];

    return (double)sum / (double)n;
}

std::vector<std::vector<Point3>> SmartScan::Filtering::SortArrays(std::vector<Point3> m_data, std::vector<std::vector<Point3>> s_data, std::vector<ReferencePoint> ref_data)
{
    // Declare a number of vectors to the point vectors for all the reference points, so we can split all data points.
    std::vector<std::vector<Point3>> vectorSet;
    for (auto i = 0; i < ref_data.size(); i++)
    {
        vectorSet.emplace_back(std::vector<Point3>());
    }

    // For each point, check the r value, whichever is smaller
    // is saved in the according vector.
    for (auto p_count = 0; p_count < m_data.size(); p_count++)
    {
        int sI = 0;
        int I = 1;
        for (auto r_count = 1; r_count < ref_data.size(); r_count++)
        {
            if (s_data[sI][p_count].s.r < s_data[I][p_count].s.r)
            {
                I++;
            }
            else if (s_data[sI][p_count].s.r > s_data[I][p_count].s.r)
            {
                sI = I;
                I++;
            }
        }

        vectorSet[sI].emplace_back(Point3(m_data[p_count].x, m_data[p_count].y, m_data[p_count].z, m_data[p_count].r.x, m_data[p_count].r.y, m_data[p_count].r.z, s_data[sI][p_count].s.r, s_data[sI][p_count].s.phi, s_data[sI][p_count].s.theta));
    }

    return vectorSet;

}






