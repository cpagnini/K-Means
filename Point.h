#ifndef POINT_H
#define POINT_H

class Point
{

public:
    Point(double coord_x, double coord_y)
    {
        this->coord_x = coord_x;
        this->coord_y = coord_y;
        id_c = 0; // id of cluster = 0;
    }

    Point()
    {
        coord_x = 0;
        coord_y = 0;
        id_c = 0; // id of cluster
    }

    double get_coord_x()
    {
        return this->coord_x;
    }

    double get_coord_y()
    {
        return this->coord_y;
    }

    int get_id_c()
    {
        return id_c; // id of cluster;
    }

    void set_id_c(int cluster_id)
    {                            // id of cluster
        this->id_c = cluster_id; // id of cluster
    }

private:
    double coord_x;
    double coord_y;
    int id_c; // id of cluster;
};

#endif