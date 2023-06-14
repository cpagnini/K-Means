#ifndef POINTSOA_H
#define POINTSOA_H

#include <vector>

struct PointSoA
{
    int size;
    std::vector<double> x_coord;
    std::vector<double> y_coord;
    std::vector<int> cluster_id;

    PointSoA(int num_points)
        : size(num_points),
          x_coord(num_points),
          y_coord(num_points),
          cluster_id(num_points)
    {
    }
};

#endif // POINTSOA_H
