# **K-Means Algorithm with C++**
A comparison between Sequential and Parallel version of K-Means with C++ using OpenMP.

![image](https://user-images.githubusercontent.com/78537430/210811423-b198b9e5-e0d1-4c60-996a-47e0ce3181c6.png)


![image](https://user-images.githubusercontent.com/78537430/208104365-09541455-46ef-4812-b2a9-bc291d81ddcc.png)

This task aim to parrtition n points into k clusters in which each point belongs to the cluster with the closer mean. It is based on the following steps:

1. Randomly generate N points
2. Randomly generate K clusters
3. For every n $\in$ N, find the distance from the nearest cluster k $\in$ K and assign the point the cluster.
4. Update the centroid's coordinates and size according with the new points added


## **Implementation**

The implementation provided for both sequential and parallel version, has same schemas.

### Point.cpp

- Randomly 2D points
  - coord_x, coord_y, id_c
- getter and setter methods for both, coordinates and cluster id methods

### Cluster.cpp

- Randomly 2D points
  - coord_x, coord_y, size, candidate_coord_x and candidate_coord_y in order to manage the accumulation of the points
  - getter and setter methods
  - add_point and update_coordinates methods to manage a new point added to the cluster and the computation of a the new centroid respectively
  
 ## Sequential version
 
 It starts by randomly generating 2D coordinates for points and clusters. It then loop calling 
 - assign_centroid which assign a point to its cluster based on the MIN Euclidean Disance
 - update_clusters

