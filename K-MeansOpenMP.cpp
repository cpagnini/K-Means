#include <cmath>
#include <fstream>
#include <chrono>
#include "Cluster.h"
#include "Point.h"
#include <omp.h>
#include<vector>
#include<iterator>
#include <sstream>
#include <iostream>
#include <unordered_map>

using namespace std;
using namespace std::chrono;


vector<Point> initalize_points(string fname);
vector<Cluster> initalize_clusters(int num_cluster,vector<Point> points); 
double distance(Point pt, Cluster cl);
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters);
void update_centroids(vector<Cluster> &clusters);

int main() {
    
    // int num_threads;
    // int num_cluster;
    // cout << "Num of clusters";
    // cin >> num_cluster;
    int num_cluster = 5;
    int num_iterations= 20;
    int num_threads =6;
    omp_set_num_threads(num_threads);
    const string fname = "C:\\Progetti\\K-Means\\datasets\\data_100000.csv";

    auto start = std::chrono::system_clock::now();
    
    vector<Point> points = initalize_points(fname);
    vector<Cluster> clusters = initalize_clusters(num_cluster, points);
    
   for(int i=0;i<num_iterations;i++){
        assign_centroid(points, clusters);
        update_centroids(clusters);
        
   }

    auto end = std::chrono::system_clock::now();
    string output;
   

    output = "";
    double duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    output = "Nr Threads: " + std::to_string(num_threads) + " Total milliseconds: " + std::to_string(duration) + "\n";
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

        vector<Point> initalize_points(string fname)
        {
            //*****************************************************************************************************************
            // INITALIZAITONS
            //*****************************************************************************************************************
            vector<Point> points;

            vector<string> row;
            string line, column;
            vector<vector<string>> content;
            fstream file(fname, ios::in);
            int num_points = 0;
            //*****************************************************************************************************************
            // FILLS VECTOR OF POINTS RANDOMLY GENERATED
            //*****************************************************************************************************************

            if (file.is_open())
            {
                while (getline(file, line))
                {
                    row.clear();
                    stringstream str(line); // Reads the file
                    while (getline(str, column, ','))
                    {                          // Takes each line and for each line each column
                        row.push_back(column); // Push each column in a vector of rows
                    }
                    content.push_back((row)); // Push each row in a matrix
                    num_points = num_points + 1;
                }
            }
            for (int i = 0; i < num_points; i++)
            {
                double coord_x = std::stod(content[i][0]);
                double coord_y = std::stod(content[i][1]);
                Point pt(coord_x, coord_y);
                points.push_back(pt);
            }
            return points;
        }

        vector<Cluster> initalize_clusters(int num_cluster, vector<Point> points)
        {
            //*****************************************************************************************************************
            // INITALIZAITONS
            //*****************************************************************************************************************
            vector<Cluster> clusters;

            //*****************************************************************************************************************
            // FILLS VECTOR OF CLUSTERS RANDOMLY GENERATED
            //*****************************************************************************************************************
            for (int i = 0; i < num_cluster; i++)
            {
                if (!points.empty())
                {
                    int n = rand() % (int)points.size();
                    Cluster cl(points[n].get_coord_x(), points[n].get_coord_y(), i);
                    clusters.push_back(cl);
                }
            }
            return clusters;
        }

        // Calculate the ecludian distance for the given Point pt and Cluster cl
        double distance(Point pt, Cluster cl)
        {

            double distance = sqrt(pow(pt.get_coord_x() - cl.get_coord_x(), 2) +
                                   pow(pt.get_coord_y() - cl.get_coord_y(), 2));

            return distance;
        }

        // Assign a point to its cluster centroid based on the MIN Euclidian Distance
        void assign_centroid(vector<Point> & points, vector<Cluster> & clusters)
        {
            double min_distance;
            int cluster_id;
            int num_points = points.size();
            int clust_size = clusters.size();

#pragma omp parallel default(shared) private(min_distance, cluster_id)

    {
#pragma omp for schedule(static)
        for (int i = 0; i < num_points; i++)
        {

            Point &p = points[i];
            Cluster temp_Cluster = clusters[0];
            min_distance = distance(p, temp_Cluster);
            cluster_id = 0;

            for (int j = 1; j < clust_size; j++)
            {
                double now_distance = distance(p, clusters[j]);
                if (now_distance < min_distance)
                {
                    min_distance = now_distance;
                    cluster_id = clusters[j].get_cluster_Id();
                }
            }

            p.set_id_c(cluster_id);
    
#pragma omp critical
        clusters[cluster_id].add_point(p);
        }
    }



}

//Update centroids
void update_centroids(vector<Cluster> &clusters){
       
        for(int i=0;i<clusters.size();i++){
            clusters[i].update_coords();
        }
        
}