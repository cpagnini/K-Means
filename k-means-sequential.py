import csv
import numpy as np
import time
import sys
import math
import matplotlib.pyplot as plt

# Function for euclidean distance
def distance_2d(x1, x2, y1, y2):
    return math.sqrt(((x1 - y1) ** 2) + ((x2 - y2) ** 2))


# Function for centroid assignment
def assign_centroid(points, centroids, points_assg):
    for i in range(counter):
        distance = sys.maxsize
        for j in range(num_clusters):
            dist = distance_2d(points[i, 0], points[i, 1], centroids[j, 0], centroids[j, 1])
            if dist < distance:
                distance = dist
                points_assg[i] = j


# Function for centroid update
def centroid_update(points, points_assgn):
    centroids_sum = np.zeros((num_clusters, 2))
    cluster_size = np.zeros(num_clusters)
    for i in range(counter):
        clust_id = points_assgn[i]
        clust_id = int(clust_id)
        cluster_size[clust_id] = cluster_size[clust_id] + 1
        centroids_sum[clust_id, 0] += points[i, 0]
        centroids_sum[clust_id, 1] += points[i, 1]
    cluster_size = np.vstack((cluster_size, cluster_size))
    return centroids_sum / cluster_size.T


def load_data(num_cluster):
    global counter
    with open('datasets/2D_data_500000.csv') as csv_file:
        data = csv.reader(csv_file)
        x_coord = []
        y_coord = []
        for item in data:
            counter = counter + 1
            x_coord.append(float(item[0]))
            y_coord.append(float(item[1]))
        points_assgn = np.zeros((counter))
        points = np.stack((x_coord, y_coord))
        points = points.T
        centroids = np.zeros((num_cluster, 2))
        for k in range(num_cluster):
            n = np.random.randint(0, points.size / 2)
            centroids[k][0] = points[n][0]
            centroids[k][1] = points[n][1]
        return centroids, points, points_assgn


iterations = 20
num_clusters = 3
counter = 0

_time = time.time()

centroids, points, points_assg = load_data(num_clusters)
for i in range(iterations):
    assign_centroid(points, centroids, points_assg)
    centroids = centroid_update(points, points_assg)
print("Seconds for clustering: " + str((time.time() - _time)))
plt.scatter(points[:, 0], points[:, 1], c=points_assg)
plt.show()
