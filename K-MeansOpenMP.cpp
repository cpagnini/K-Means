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

void setting_environment();
vector<Point> initalize_points(string fname);
vector<Cluster> initalize_clusters(int num_cluster,vector<Point> points); 
double distance(Point pt, Cluster cl);
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters);
void update_centroids(vector<Cluster> &clusters);
void draw_chart_gnu(vector<Point> &points);

int main() {
    setting_environment();
    int num_threads;
    int num_cluster;
    cout << "Num of clusters";
    cin >> num_cluster;
    //int num_cluster = 5;
    int num_iterations= 20;
    num_threads =2;
    omp_set_num_threads(num_threads);
    const string fname = "/datasets/data_500000.csv";

    auto start = std::chrono::system_clock::now();
    
    vector<Point> points = initalize_points(fname);
    vector<Cluster> clusters = initalize_clusters(num_cluster, points);
    
   for(int i=0;i<num_iterations;i++){
        assign_centroid(points, clusters);
        update_centroids(clusters);
        
   }

    auto end = std::chrono::system_clock::now();
    string output;
    int cluster_size = clusters.size();
    // for (int i = 0; i < cluster_size; i++)
    // {
    //     double x = clusters[i].get_coord_x();
    //     double y = clusters[i].get_coord_y();
    //     output += "cluster points: \n coord x: " + std::to_string(x) + " coord y: " + std::to_string(y) + "\n";
    // }
    // // Open the file in output mode
    // std::ofstream outputFile("Clusters.txt", std::ios::app);

    // // Check if the file was opened successfully
    // if (outputFile.is_open())
    // {
    //     // Write the string to the file
    //     outputFile << output;

    //     // Close the file
    //     outputFile.close();
    // }

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
void setting_environment(){
        string filename = "config.json";
        // Read the JSON file
        std::ifstream jsonFile(filename);
        if (!jsonFile.is_open())
        {
        std::cout << "Error opening JSON file: " << filename << std::endl;
        return;
        }

        // Read the JSON data as a string
        std::stringstream jsonStream;
        jsonStream << jsonFile.rdbuf();
        jsonFile.close();

        std::string jsonData = jsonStream.str();

        // Parse the JSON data
        std::unordered_map<std::string, std::string> jsonMap;
        size_t startPos = 0;
        while (startPos < jsonData.length())
        {
        size_t keyPos = jsonData.find("\"", startPos);
        if (keyPos == std::string::npos)
            break;

        size_t endKeyPos = jsonData.find("\"", keyPos + 1);
        if (endKeyPos == std::string::npos)
            break;

        size_t valuePos = jsonData.find("\"", endKeyPos + 1);
        if (valuePos == std::string::npos)
            break;

        size_t endValuePos = jsonData.find("\"", valuePos + 1);
        if (endValuePos == std::string::npos)
            break;

        std::string key = jsonData.substr(keyPos + 1, endKeyPos - keyPos - 1);
        std::string value = jsonData.substr(valuePos + 1, endValuePos - valuePos - 1);

        jsonMap[key] = value;

        startPos = endValuePos + 1;
        }

        // Set environment variables
        for (const auto &pair : jsonMap)
        {
        std::string envVar = pair.first + "=" + pair.second;
        
        }
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