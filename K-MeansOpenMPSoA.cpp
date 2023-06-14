#include <cmath>
#include <fstream>
#include <chrono>
#include "PointSoA.h"
#include "ClusterSoA.h"
#include <omp.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

vector<PointSoA> initialize_points(string fname);
vector<ClusterSoA> initialize_clusters(int num_clusters, const vector<PointSoA> &points);
double distance(const PointSoA &point, const ClusterSoA &cluster, int point_index, int cluster_index);
void assign_centroid(vector<PointSoA> &points, vector<ClusterSoA> &clusters);
void update_centroids(vector<ClusterSoA> &clusters);
void draw_chart_gnu(vector<PointSoA> &points);

int main()
{
    int num_clusters = 5;
    int num_iterations = 20;
    int num_threads = 1;
    omp_set_num_threads(num_threads);
    const string fname = "C:\\Progetti\\K-Means\\datasets\\data_100000.csv";

    auto start = std::chrono::system_clock::now();

    vector<PointSoA> points = initialize_points(fname);
    vector<ClusterSoA> clusters = initialize_clusters(num_clusters, points);

    for (int i = 0; i < num_iterations; i++)
    {
        assign_centroid(points, clusters);
        update_centroids(clusters);
    }

    auto end = std::chrono::system_clock::now();
    string output;
    output = "";
    double duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    output = "SoA ---- ";
    output += "Nr Threads: " + std::to_string(num_threads) + " Total milliseconds: " + std::to_string(duration) + "\n";
    // Open the file in output mode
    std::ofstream outputFile("Result.txt", std::ios::app);

    // Check if the file was opened successfully
    if (outputFile.is_open())
    {
        // Write the string to the file
        outputFile << output;

        // Close the file
        outputFile.close();
    }

    return 0;
}

vector<PointSoA> initialize_points(string fname)
{
    vector<PointSoA> points;
    vector<string> row;
    string line, column;
    vector<vector<string>> content;
    fstream file(fname, ios::in);
    int num_points = 0;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            row.clear();
            stringstream str(line);
            while (getline(str, column, ','))
            {
                row.push_back(column);
            }
            content.push_back(row);
            num_points++;
        }
        file.close();
    }

    PointSoA pointSoA(num_points);

    for (int i = 0; i < num_points; i++)
    {
        double coord_x = std::stod(content[i][0]);
        double coord_y = std::stod(content[i][1]);
        pointSoA.x_coord[i] = coord_x;
        pointSoA.y_coord[i] = coord_y;
    }

    points.push_back(std::move(pointSoA)); // Use std::move to avoid unnecessary copying

    return points;
}

vector<ClusterSoA> initialize_clusters(int num_clusters, const vector<PointSoA> &points)
{
    vector<ClusterSoA> clusters;
    ClusterSoA clusterSoA(num_clusters);

    for (int i = 0; i < num_clusters; i++)
    {
        if (!points.empty())
        {
            int n = rand() % static_cast<int>(points[0].size);
            clusterSoA.coord_x[i] = points[0].x_coord[n];
            clusterSoA.coord_y[i] = points[0].y_coord[n];
        }
    }

    clusters.push_back(std::move(clusterSoA)); // Use std::move to avoid unnecessary copying

    return clusters;
}

double distance(const PointSoA &point, const ClusterSoA &cluster, int point_index, int cluster_index)
{
    double diff_x = point.x_coord[point_index] - cluster.coord_x[cluster_index];
    double diff_y = point.y_coord[point_index] - cluster.coord_y[cluster_index];
    return sqrt(diff_x * diff_x + diff_y * diff_y);
}

void assign_centroid(vector<PointSoA> &points, vector<ClusterSoA> &clusters)
{
    int num_points = points[0].size;
    int num_clusters = clusters[0].size;

#pragma omp parallel for
    for (int i = 0; i < num_points; i++)
    {
        int closest_cluster = 0;
        double min_distance = distance(points[0], clusters[0], i, 0);

        for (int j = 1; j < num_clusters; j++)
        {
            double dist = distance(points[0], clusters[0], i, j);
            if (dist < min_distance)
            {
                min_distance = dist;
                closest_cluster = j;
            }
        }

        points[0].cluster_id[i] = closest_cluster;
    }

    // Reassign points to clusters
    for (int i = 0; i < num_clusters; i++)
    {
        clusters[0].point_count[i] = 0;
        clusters[0].coord_x[i] = 0.0;
        clusters[0].coord_y[i] = 0.0;
    }

    for (int i = 0; i < num_points; i++)
    {
        int cluster_id = points[0].cluster_id[i];
        clusters[0].point_count[cluster_id]++;
        clusters[0].coord_x[cluster_id] += points[0].x_coord[i];
        clusters[0].coord_y[cluster_id] += points[0].y_coord[i];
    }
}

void update_centroids(vector<ClusterSoA> &clusters)
{
    int num_clusters = clusters[0].size;

    for (int i = 0; i < num_clusters; i++)
    {
        if (clusters[0].point_count[i] > 0)
        {
            clusters[0].coord_x[i] /= clusters[0].point_count[i];
            clusters[0].coord_y[i] /= clusters[0].point_count[i];
        }
    }
}
