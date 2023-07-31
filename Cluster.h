#ifndef CLUSTER_H
#define CLUSTER_H
#include "Point.h"

class Cluster
{
public:
    Cluster(double coord_x, double coord_y, int id)
    {
        candidate_coord_x = 0;
        candidate_coord_y = 0;
        size = 0;
        this->coord_x = coord_x;
        this->coord_y = coord_y;
        this->id = id;
    }

    Cluster()
    {
        candidate_coord_x = 0;
        candidate_coord_y = 0;
        size = 0;
        this->coord_x = 0;
        this->coord_y = 0;
    }

    void add_point(Point point)
    {
#pragma omp atomic
        candidate_coord_x += point.get_coord_x();
#pragma omp atomic
        candidate_coord_y += point.get_coord_y();
#pragma omp atomic

        size++;
    }

    double get_coord_x()
    {
        return this->coord_x;
    }

    double get_coord_y()
    {
        return this->coord_y;
    }

    void update_coords()
    {
        this->coord_x = candidate_coord_x / this->size;
        this->coord_y = candidate_coord_y / this->size;
        
    }
    void free_point()
    {
        this->size = 0;
        this->candidate_coord_x = 0;
        this->candidate_coord_y = 0;
    }
    void set_cluster_Id(int cluster_id)
    {
        this->id = cluster_id;
    }

    int get_cluster_Id()
    {
        return id;
    }

    int get_size()
    {
        return size;
    }

private:
    double coord_x;
    double coord_y;
    double candidate_coord_x;
    double candidate_coord_y;
    int size;
    int id;

    
};

#endif