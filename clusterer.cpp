#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <random>
#include <algorithm>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <memory>
#include "clusterer.h"
#include "grey_feature.h"
#include "colour_feature.h"
#include "two_d_feature.h"
#include "shape_feature.h"
 


using namespace BLLSAM009;


Clusterer::Clusterer() // default constructor 
{
    
}

Clusterer::Clusterer(Grey_Feature grey) // greyscale constructor 
{
    image_features = grey.get_image_features();
    image_names = grey.get_image_names();
    
}


Clusterer::Clusterer(Colour_Feature colour) // colour constructor
{
     image_features = colour.get_image_features();
     image_names = colour.get_image_names();  
}



Clusterer::Clusterer(Two_D_Feature two_dim) // two dimensional constructor
{
     image_features = two_dim.get_image_features();
     image_names = two_dim.get_image_names();  
}


Clusterer::Clusterer(Shape_Feature shape) // shape constructor
{
     image_features = shape.get_image_features();
     image_names = shape.get_image_names();  
}


Clusterer::~Clusterer()
{
    //clean up memory
      for (int i = 0; i < cluster_means.size(); i++)
    {
        delete [] cluster_means[i];
    }
}



//improved initialisation method kmeans++
void Clusterer::k_meansplusplus_init(const int no_clusters, const int hist_size)
{

    std::cout << "Get initial cluster means for " << no_clusters << " clusters" << std::endl;
    srand(time(NULL)); //random seed
    int no_features = image_features.size();

    //keep track of features that have already been chosen
    int indices[no_features];
    for (int i = 0; i < no_features; i++)
    {
        indices[i] = 0;
    }

    //choose first random initial mean
    int random = (rand() % image_features.size());
    if(indices[random]==0)  //check that feature has not already been chosen
    {
        indices[random] = 1; //mark as chosen
        cluster_means.push_back(image_features[random]); 
        cluster_indexes.push_back(random);
    }

    //select feature having the maximum distance from the nearest centroid
    int max = 0;
    int new_centroid = -1;
    for(int k = 0; k < (no_clusters-1); k++)
    {
        float max = 0;
        for (int i = 0; i < image_features.size(); i++)
        {
            float min_distance = 10000000;
            if(indices[i]==0) //hasn't been chosen as centroid yet
            {
                //for each cluster, find which feature is closest to it
                for (int j = 0; j < cluster_means.size(); j++)
                {
                
                    float distance = get_abs_distance(i, j, hist_size); 
                    if(distance < min_distance)
                    {
                        min_distance = distance;
                    }
                 
                }
                if(min_distance > max)
                {
                    max = min_distance;
                    new_centroid = i; //furthest distance
                }
            }

        }
        indices[new_centroid] = 1; //mark that this has been used
        cluster_means.push_back(image_features[new_centroid]); //add to cluster means
        cluster_indexes.push_back(new_centroid);
    }

    //display initial cluster means
    std::cout << "\nINITIAL CLUSTER MEANS" << std::endl;
    for (int i  = 0 ; i < cluster_means.size(); i++)
    {
        std::cout << image_names[cluster_indexes[i]] << std::endl;
    }

}




//Randomly chooses k observations from the dataset and uses these as the initial means
void Clusterer::get_random_means(const int no_clusters)
{
    
    std::cout << "Get initial cluster means for " << no_clusters << " clusters" << std::endl;
    srand(time(NULL));
    //srand(1000); //quite a good choice
    int no_features = image_features.size();

    //keep track of features that have already been chosen
    int indices[no_features];
    for (int i = 0; i < no_features; i++)
    {
        indices[i] = 0;
    }
    for (int i  = 0 ; i < no_clusters;)
    {
        int random = (rand() % image_features.size());
        if(indices[random]==0)
        {
            indices[random] = 1;
            cluster_means.push_back(image_features[random]); 
            std::cout << random << " " << std::endl;
            std::cout << "initial cluser mean " << i << ": " << image_names[random] << std::endl; 
            i++;
        }
    }
   
    //display resulting cluster means
    std::cout << "\nINITIAL CLUSTER MEANS" << std::endl;
    for (int i  = 0 ; i < cluster_means.size(); i++)
    {
        for(int j =0; j < (26); j++)
        {
            std::cout << cluster_means[i][j] << ", ";
        }
        std::cout << "\n";
    }
    std::cout << cluster_means.size() << std::endl;


}


//euclidean distance between centroids and features 
float Clusterer::get_euclid_distance(const int image_index, const int cluster_index, const int hist_size) const
{
    
    float sum = 0;
    for (int i = 0; i < hist_size; i++)
    {
        sum += std::pow((image_features[image_index][i] - cluster_means[cluster_index][i]), 2); 
    }
    sum = std::sqrt(sum); 
    return sum;

}


//absolute distance between centroids and features
float Clusterer::get_abs_distance(const int image_index, const int cluster_index, const int hist_size) const
{
    
    float sum = 0;
    for (int i = 0; i < hist_size; i++)
    {
        sum += std::pow(std::abs((image_features[image_index][i] - cluster_means[cluster_index][i])), 1); 

    }
    return sum;

}




//Assign each observation to the cluster with the nearest mean - with the least squared Euclidean distance
void Clusterer::assign_to_cluster(const int hist_size)
{
    std::cout << "\nAssigning to clusters... " << std::endl;
    cluster_map.clear(); //refresh map
    int num_clusters = cluster_means.size();
  
    for(int i = 0; i < image_features.size(); i++)
    {
       
        float min = 100000000000; //initialise to very large number 
        int closest_cluster = -1; //set to -1 to indicate no closest cluster found
        for (int j=0; j < num_clusters; j++)
        {

            float distance = get_euclid_distance(i, j, hist_size);

            if(distance < min)
            {
                min = distance;
                closest_cluster = j;
            }
        }
        cluster_map[closest_cluster].push_back(i); //assign index of image to that cluster
    }
    
}


//Recalculate means (centroids) for observations assigned to each cluster
void Clusterer::update_means(const int hist_size)
{
    std::cout << "Updating means..." << std::endl;
    has_converged = true; //must check for every cluster 
    //iterate through clusters
    for (int i = 0; i < cluster_means.size(); i++)
    {
        //keep track of whether cluster means have changed
        bool has_changed = update_bin_avgs(i, hist_size);
        if(has_changed == true)
        {
            has_converged = false;
        }
    }

}



//computes new bin averages for a single cluster mean
bool Clusterer::update_bin_avgs(const int cluster_index, const int hist_size)
{
    bool has_changed = false;
    //get pixels associated with the cluster index
    int * new_bin_avgs = new int[hist_size];
    if (!(cluster_map.find(cluster_index) == cluster_map.end())) //check if has any values
    {
    std::vector<int> image_indexes = cluster_map.at(cluster_index);
    int * old_bin_avgs = cluster_means[cluster_index];
    for(int i = 0; i < hist_size; i++) //for a certain bin
    {   
        float sum = 0;
        for(int j = 0; j < image_indexes.size(); j++) //average all values of that bin over all images in the cluster
        {
             sum += image_features[image_indexes[j]][i]; //get ith bin of jth image feature
        }
        int bin_avg = std::round(sum/image_indexes.size());
        new_bin_avgs[i] = bin_avg;

        //check for convergence
        if(old_bin_avgs[i] != bin_avg) //if average has changed, the algorithm has not converged
        {
            has_changed = true;
        }
        
    }
    cluster_means[cluster_index] = new_bin_avgs;
    }
    return has_changed;
    
}





//Call assign to cluster and update means iteratively until no further changes observed
void Clusterer::k_means(const int no_clusters, const int hist_size)
{
    //initialise cluster means
    //get_random_means(no_clusters);
    k_meansplusplus_init(no_clusters, hist_size); //improved initialisation
    has_converged = false;
    //while averages are still changing
    while(!(has_converged))
    {
        assign_to_cluster(hist_size); //assign each image to a cluster
        update_means(hist_size); //update cluster means based on new assignment

    }
    
}


//friend operator
std::ostream & BLLSAM009::operator<<(std::ostream & os, const Clusterer & kt)
{
    for (auto const& x : kt.cluster_map)
    {
        os << "Cluster " << x.first << ": " << std::endl;
        os << kt.image_names[x.second[0]]; 
        for(int i = 1; i < x.second.size(); i++)
        {
            os << ", \n" + kt.image_names[x.second[i]]; //display clusters
        }
        os << "\n" << std::endl;
    }
    return os;

}













