#ifndef CLUSTERSOA_H
#define CLUSTERSOA_H

#include <vector>

struct ClusterSoA
{
    int size;
    std::vector<double> coord_x;
    std::vector<double> coord_y;
    std::vector<int> point_count;

    ClusterSoA(int num_clusters)
        : size(num_clusters),
          coord_x(num_clusters),
          coord_y(num_clusters),
          point_count(num_clusters)
    {
    }
};

#endif // CLUSTERSOA_H