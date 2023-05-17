#include <cmath>
#include <fstream>
#include <chrono>
#include "/content/drive/My Drive/Parallel/K-means/Cluster.h"
#include "/content/drive/My Drive/Parallel/K-means/Point.h"
#include <omp.h>
#include<vector>
#include<iterator>
#include <sstream>
#include <iostream>



using namespace std;
using namespace std::chrono;


vector<Point> initalize_points(string fname);
vector<Cluster> initalize_clusters(int num_cluster,vector<Point> points); 
double distance(Point pt, Cluster cl);
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters);
void update_centroids(vector<Cluster> &clusters);
void draw_chart_gnu(vector<Point> &points);

int main() {
    int num_threads;
    int num_cluster=5;
    int num_iterations= 20;
    num_threads =2;
    omp_set_num_threads(num_threads);
    const string fname ="/content/drive/My Drive/Parallel/K-means/2D_data_100.csv";

    auto start = std::chrono::system_clock::now();
    
    vector<Point> points = initalize_points(fname);
    vector<Cluster> clusters = initalize_clusters(num_cluster, points);
    
   for(int i=0;i<num_iterations;i++){
        assign_centroid(points, clusters);
        update_centroids(clusters);
        
   }

    auto end = std::chrono::system_clock::now();
    double duration = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    cout<<"Total milliseconds: "<< duration<< endl;
    

    return 0;
}

vector <Point> initalize_points(string fname){
    //*****************************************************************************************************************
    //INITALIZAITONS
    //*****************************************************************************************************************
    vector<Point> points;

    vector<string>row;
    string line, column;
    vector<vector<string>> content;
    fstream file(fname, ios::in);
    int num_points = 0;
    //*****************************************************************************************************************
    //FILLS VECTOR OF POINTS RANDOMLY GENERATED
    //*****************************************************************************************************************

    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line); //Reads the file
            while(getline(str, column, ',')){ //Takes each line and for each line each column
                row.push_back(column); //Push each column in a vector of rows
            }
            content.push_back((row)); //Push each row in a matrix
            num_points=num_points+1;

        }

    }
    for(int i=0;i<num_points; i++){
        double coord_x = std::stod(content[i][0]);
        double coord_y = std::stod(content[i][1]);
        Point pt (coord_x,coord_y );
        points.push_back(pt);
        
    }
    return points;
}

vector <Cluster> initalize_clusters(int num_cluster,vector<Point> points){
    //*****************************************************************************************************************
    //INITALIZAITONS
    //*****************************************************************************************************************
    vector<Cluster> clusters;

    //*****************************************************************************************************************
    //FILLS VECTOR OF CLUSTERS RANDOMLY GENERATED
    //*****************************************************************************************************************
    for(int i=0;i<num_cluster; i++){
        int n = rand() % (int)points.size();
        Cluster cl (points[n].get_coord_x(), points[n].get_coord_y(), i);
        clusters.push_back(cl);
    }
    return clusters;
}

//Calculate the ecludian distance for the given Point pt and Cluster cl
double distance(Point pt, Cluster cl){

    double distance = sqrt(pow(pt.get_coord_x() - cl.get_coord_x(),2) +
                           pow(pt.get_coord_y() - cl.get_coord_y(), 2));

    return distance;
}

//Assign a point to its cluster centroid based on the MIN Euclidian Distance
void assign_centroid(vector<Point> &points, vector<Cluster> &clusters){
    //*****************************************************************************************************************
    //Double loop points and clusters in order to find the MIN distance between those two. It then assign the point to the cluster which is closer.
    //*****************************************************************************************************************
    double min_distance;
    #pragma omp parallel
    #pragma omp for
    for(Point& p: points){
        Cluster temp_Cluster = clusters[0];
        min_distance = distance(p, temp_Cluster);
        int cluster_id = 0; //Necessary in case there is not better distance and cluster_id is not updated
        for(int i = 1;i<clusters.size();i++){
            double now_distance = distance(p, clusters[i]);
            if(now_distance<min_distance){
                min_distance = now_distance;
                cluster_id = clusters[i].get_cluster_Id();
                
            }
        }
        p.set_id_c(cluster_id); //The cluster id identify the position of this cluster in the clusters array
        #pragma omp crtical
        clusters[cluster_id].add_point(p);
    }
   
}



//Update centroids
void update_centroids(vector<Cluster> &clusters){
       
        for(int i=0;i<clusters.size();i++){
            clusters[i].update_coords();
        }
        
}