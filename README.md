# **K-Means Algorithm with C++**
A comparison between Sequential and Parallel version of K-Means with C++ using OpenMP.

![image](https://user-images.githubusercontent.com/78537430/210811423-b198b9e5-e0d1-4c60-996a-47e0ce3181c6.png)


![image](https://user-images.githubusercontent.com/78537430/208104365-09541455-46ef-4812-b2a9-bc291d81ddcc.png)

This task aim to parrtition n points into k clusters in which each point belongs to the cluster with the closer mean. It is based on the following steps:

1. Randomly generate N points
2. Randomly generate K clusters
3. For every n $\in$ N, find the distance from the nearest cluster k $\in$ K and assign the point the cluster.
4. Update the centroid's coordinates and size according with the new points added

# Implementations
### 1. Sequential Version
The sequential version of the K-Means algorithm is a popular unsupervised machine learning algorithm used for data clustering and pattern recognition. It aims to partition data points into K clusters, represented by centroids. The main steps include initialization, assignment of data points to the nearest centroid, updating centroids, and iterative clustering until convergence.

### 2. Parallel Version using Array of Structures (AoS)
The parallel version of the K-Means algorithm using Array of Structures (AoS) is implemented in C++ with the help of OpenMP. This approach uses parallelism to speed up the computation of the clustering process. The points are organized in an array of structures, and multiple threads work on different points simultaneously to achieve parallel processing.

### 3. Parallel Version using Structure of Arrays (SoA)
The parallel version of the K-Means algorithm using Structure of Arrays (SoA) is another approach implemented in C++ with OpenMP. In this implementation, data points are organized into separate arrays based on attributes, and each thread works on a specific attribute independently. This can provide better performance in certain scenarios, especially for large datasets and optimized memory access.

## Point and Cluster.h

### 1. AoS (Array of Structures) Version:
In the AoS version of K-Means, the data points are represented as a single data structure (usually an array), where each element of the array contains all the attributes (dimensions) of a single data point. In other words, the attributes of a data point are stored together in a contiguous block of memory. For example, in a 2D dataset, the AoS representation would have an array where each element holds both the "x" and "y" coordinates of a data point. This version is intuitive and convenient for accessing individual data points but may suffer from memory access inefficiencies when performing certain calculations.

### 2. SoA (Structure of Arrays) Version:
In the SoA version of K-Means, the data points are represented using multiple arrays, where each array stores the values of a specific attribute (dimension) for all data points. In this organization, the "x" coordinates of all data points are stored in one array, and the "y" coordinates are stored in another array. This means that the attributes of all data points are separated into distinct arrays. The SoA version can improve memory access patterns, especially when performing vectorized operations or parallel computations, but may require additional effort to manage multiple arrays and access individual data points.

# Dataset
You can provide your own dataset in a CSV file format, where each row represents a data point with its features. The code will read the dataset from the CSV file and perform the clustering accordingly.

# Contributing
Contributions to this repository are welcome! If you find any issues, have suggestions for improvements, or want to add more parallelization techniques, feel free to open a pull request.

# Acknowledgments
The implementation of the Meanshift algorithm is inspired by academic papers and existing implementations. We acknowledge the contributions of researchers in the field and the resources available online that have been used to develop this project.
