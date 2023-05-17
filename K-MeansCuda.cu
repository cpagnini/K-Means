#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <cuda_runtime.h>


#define N 500000
#define K 5
#define MAX_ITER 20
#define TPB 128
#define EPSILON 0.00001


__device__ float Euclidean_Distance(const float x1, const float x2, const float y1, const float y2)
{
    return sqrt(pow((x1-y1),2) + pow((x2-y2),2));
}


__global__ void Cluster_Assignment(const float *d_Point_Coord_x, const float *d_Point_Coord_y, int *d_Cluster_Membership, const float *d_Centroid_Coord_X, const float *d_Centroid_Coord_Y)
{
    //get idx for this datapoint
    const int idx = blockIdx.x*blockDim.x + threadIdx.x;

    if (idx >= N) return;


    //find the closest centroid to this datapoint
    float min_dist = INFINITY;
    int closest_centroid = 0;

    for(int c = 0; c<K; ++c)

    {
        float dist = Euclidean_Distance(d_Point_Coord_x[idx], d_Point_Coord_y[idx], d_Centroid_Coord_X[c], d_Centroid_Coord_Y[c]);

        if(dist < min_dist)
        {
            min_dist = dist;
            closest_centroid=c;
        }
    }

    //assign closest cluster id for this datapoint/thread
    d_Cluster_Membership[idx]=closest_centroid;
}
__global__ void Sum_Update(const float *d_Point_Coord_x, const float *d_Point_Coord_y, const int *d_Cluster_Membership, float *d_Centroid_sum_x, float *d_Centroid_sum_y, int *d_Cluster_Size) {

    //get idx of thread at grid level
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= N) return;

    int clust_id = d_Cluster_Membership[idx];

    atomicAdd(&(d_Centroid_sum_x[clust_id]), d_Point_Coord_x[idx]);
    atomicAdd(&(d_Centroid_sum_y[clust_id]), d_Point_Coord_y[idx]);
    atomicAdd(&(d_Cluster_Size[clust_id]), 1);


}

__host__ void read_data(const char *file_name, float *h_Point_Coord_x, float *h_Point_Coord_y) {
    FILE *file;
    file = fopen(file_name, "r");
    //printf("%s\n", file_name);
    //initalize datapoints from csv
    printf("DataPoints: \n");
    for(int i=0;i<N;++i){
        fscanf(file,"%f,%f\n", &h_Point_Coord_x[i], &h_Point_Coord_y[i]);
        //printf("(%f, %f) \n",  h_Point_Coord_x[i], h_Point_Coord_y[i]);
    }
    fclose(file);
}
                 
__host__ void initalize_cluster(float *h_Point_Coord_x,float *h_Point_Coord_y,float *h_Centroid_Coord_X,float *h_Centroid_Coord_Y, float *h_Centroid_sum_x, float *h_Centroid_sum_y, int *h_Cluster_Size){
    
    //printf("Clusters: \n");
    for(int i=0;i<K;++i){
        int r = rand() % N;
        h_Centroid_sum_x[i]=0.0;
        h_Centroid_sum_y[i]=0.0;
        h_Cluster_Size[i]=0;
        h_Centroid_Coord_X[i] = h_Point_Coord_x[r];
        h_Centroid_Coord_Y[i] = h_Point_Coord_y[r];
        
    }
}
int main()
{
    
    srand(time(NULL));   
    
    const char *file_name = "/content/drive/My Drive/Parallel/K-means/Cuda/2D_data_1000.csv";

    //allocate memory on the device for the data points
    float *d_Point_Coord_x;
    float *d_Point_Coord_y;
    //allocate memory on the device for the cluster assignments
    int *d_Cluster_Membership;
    //allocate memory on the device for the cluster centroids
    float *d_Centroid_sum_x;
    float *d_Centroid_sum_y;
    float *d_Centroid_Coord_X;
    float *d_Centroid_Coord_Y;
    //allocate memory on the device for the cluster sizes
    int *d_Cluster_Size;

    cudaMalloc(&d_Point_Coord_x, N*sizeof(float));
    cudaMalloc(&d_Point_Coord_y, N*sizeof(float));
    cudaMalloc(&d_Cluster_Membership,N*sizeof(int));
    cudaMalloc(&d_Centroid_sum_x,K*sizeof(float));
    cudaMalloc(&d_Centroid_sum_y,K*sizeof(float));
    cudaMalloc(&d_Centroid_Coord_X,K*sizeof(float));
    cudaMalloc(&d_Centroid_Coord_Y,K*sizeof(float));
    cudaMalloc(&d_Cluster_Size,K*sizeof(int));

    //allocate memory for host
    float *h_Centroid_Coord_X = (float*)malloc(K*sizeof(float));
    float *h_Centroid_Coord_Y = (float*)malloc(K*sizeof(float));
    float *h_Centroid_sum_x = (float*)malloc(K*sizeof(float));
    float *h_Centroid_sum_y = (float*)malloc(K*sizeof(float));
    float *h_Point_Coord_x = (float*)malloc(N*sizeof(float));
    float *h_Point_Coord_y = (float*)malloc(N*sizeof(float));
    int *h_Cluster_Membership = (int*)malloc(N*sizeof(int));
    int *h_Cluster_Size = (int*)malloc(K*sizeof(int));


    //Read data from CSV
    read_data(file_name,h_Point_Coord_x,h_Point_Coord_y);
    //Initialization of cluster from datapoints
    initalize_cluster(h_Centroid_Coord_X,h_Centroid_Coord_Y,h_Centroid_Coord_X,h_Centroid_Coord_Y, h_Centroid_sum_x, h_Centroid_sum_y, h_Cluster_Size);



    //copy datapoints and all other data from host to device
    cudaMemcpy(d_Centroid_Coord_X,h_Centroid_Coord_X,K*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Centroid_Coord_Y,h_Centroid_Coord_Y,K*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Centroid_sum_x,h_Centroid_sum_x,K*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Centroid_sum_y,h_Centroid_sum_y,K*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Point_Coord_x,h_Point_Coord_x,N*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Point_Coord_y,h_Point_Coord_y,N*sizeof(float),cudaMemcpyHostToDevice);
    cudaMemcpy(d_Cluster_Size,h_Cluster_Size,K*sizeof(int),cudaMemcpyHostToDevice);



    //Start time for clustering
    clock_t start = clock();
    int cur_iter = 0;
    while(cur_iter < MAX_ITER)
    {
        printf("Iter %d: \n",cur_iter);
        //Start time for iteration
        clock_t start_iter = clock();


        //Points assg
        Cluster_Assignment<<<(N+TPB-1)/TPB, TPB>>>(d_Point_Coord_x, d_Point_Coord_y, d_Cluster_Membership, d_Centroid_Coord_X, d_Centroid_Coord_Y);


        //reset centroids and cluster sizes (will be updated in the next kernel)
        cudaMemset(d_Centroid_sum_x,0.0,K*sizeof(float));
        cudaMemset(d_Centroid_sum_y,0.0,K*sizeof(float));

        //call centroid update
        Sum_Update<<<(N+TPB-1)/TPB, TPB>>>(d_Point_Coord_x, d_Point_Coord_y, d_Cluster_Membership, d_Centroid_sum_x, d_Centroid_sum_y, d_Cluster_Size);

        //Copy centroids sum and clusters sizes back to host
        cudaMemcpy(h_Centroid_sum_x,d_Centroid_sum_x,K*sizeof(float),cudaMemcpyDeviceToHost);
        cudaMemcpy(h_Centroid_sum_y,d_Centroid_sum_y,K*sizeof(float),cudaMemcpyDeviceToHost);
        cudaMemcpy(h_Cluster_Size,d_Cluster_Size,K*sizeof(int),cudaMemcpyDeviceToHost);

        cudaMemset(d_Cluster_Size,0,K*sizeof(int));
        for(int i=0; i < K; i++){
            h_Centroid_Coord_X[i]=h_Centroid_sum_x[i]/h_Cluster_Size[i];
            h_Centroid_Coord_Y[i]=h_Centroid_sum_y[i]/h_Cluster_Size[i];
        }
        for(int i=0; i < K; i++){
            //printf("C %d: (%f, %f)\n",i,h_Centroid_Coord_X[i],h_Centroid_Coord_Y[i]);
        }

        //Stop time for iteration
        clock_t end_iter = clock();
        float seconds_iter = (float)(end_iter - start_iter) / CLOCKS_PER_SEC/1000;
        //printf("Time for iter: %f\n", seconds_iter);

        //Compare the centroids for stop the clustering
        cudaMemcpy(d_Centroid_Coord_X,h_Centroid_Coord_X,K*sizeof(float),cudaMemcpyHostToDevice);
        cudaMemcpy(d_Centroid_Coord_Y,h_Centroid_Coord_Y,K*sizeof(float),cudaMemcpyHostToDevice);

        cur_iter+=1;
    }

    cudaMemcpy(h_Cluster_Membership,d_Cluster_Membership,N*sizeof(int),cudaMemcpyDeviceToHost);

    clock_t end = clock();
    float milliseconds = (float)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("Time for clustering: %f milliseconds \n", milliseconds);
    printf("Time for average iteration: %f milliseconds\n", milliseconds / MAX_ITER);
    FILE *res;

    res = fopen("/content/drive/My Drive/Parallel/K-means/Cuda/2D_data_3_results.csv", "w+");
    for(int i=0;i<N;i++){
        fprintf(res,"%d\n", h_Cluster_Membership[i]);
    }

    cudaFree(d_Point_Coord_x);
    cudaFree(d_Point_Coord_y);
    cudaFree(d_Cluster_Membership);
    cudaFree(d_Centroid_Coord_X);
    cudaFree(d_Centroid_Coord_Y);
    cudaFree(d_Cluster_Size);

    free(h_Centroid_Coord_X);
    free(h_Centroid_Coord_Y);
    free(h_Point_Coord_x);
    free(h_Point_Coord_y);
    free(h_Cluster_Membership);
    free(h_Cluster_Size);

    return 0;
}