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
#include "three_d_feature.h"
#include "shape_feature.h"
 
 
 //use of a simple unsupervised classification scheme – K-means clustering 
 // – to classify images into different categories/types
 //will require that some information is provided – the number of image classes (K)


//These greyscale images will be used for feature generation, 
//and the features will be used to map each image to a class during classification. 


//NB! Check that memory is managed correctly!!

using namespace BLLSAM009;


Clusterer::Clusterer() // default constructor 
{
    
}

Clusterer::Clusterer(Grey_Feature grey) // greyscale constructor 
{
    image_features = grey.get_image_features();
    image_names = grey.get_image_names();
    int no_features = image_features.size();
     std::cout << no_features << std::endl;
    
}


Clusterer::Clusterer(Colour_Feature colour) // colour constructor
{
     image_features = colour.get_image_features();
     image_names = colour.get_image_names();  
     int no_features = image_features.size();
}



Clusterer::Clusterer(Two_D_Feature two_dim) // colour constructor
{
     image_features = two_dim.get_image_features();
     image_names = two_dim.get_image_names();  
     int no_features = image_features.size();
}


Clusterer::Clusterer(Three_D_Feature three_dim) // colour constructor
{
     image_features = three_dim.get_image_features();
     image_names = three_dim.get_image_names();  
     int no_features = image_features.size();
     std::cout << "no features " << no_features << std::endl;
}


Clusterer::Clusterer(Shape_Feature shape) // colour constructor
{
     image_features = shape.get_image_features();
     image_names = shape.get_image_names();  
     int no_features = image_features.size();
     std::cout << "no features " << no_features << std::endl;
}


Clusterer::~Clusterer()
{
    //clean everything properly OR use special pointers
}


void Clusterer::print_hists(int i, int k, int hist_size)
{
    
    std::cout << "Image " << image_names[i] << std::endl;
    for(int j =0; j < (hist_size); j++)
    {
            std::cout << image_features[i][j] << ", ";
    }
    std::cout << "\n";
    std::cout << "Image " << image_names[k] << std::endl;
    for(int j =0; j < (hist_size); j++)
    {
            std::cout << image_features[k][j] << ", ";
    }
    std::cout << "\n";
    
}


void Clusterer::k_meansplusplus_init(const int no_clusters, const int hist_size)
{
    
   // std::vector<int> cluster_indexes;
    std::cout << "Get initial cluster means for " << no_clusters << " clusters" << std::endl;
    //int seed = 0; //or use clock to set this
    //use a seed that gives you good results
    //somehow make sure the random means are distributed !!
    srand(time(NULL));
    //srand(1); //CHOSEN FOR BEST PERFORMANCE WITH NORMAL COLOUR AND GREY
    //srand(10);
    //std::vector<int> indices;
    int no_features = image_features.size();
     std::cout << "No.features" << no_features << std::endl;
    int indices[no_features];
    for (int i = 0; i < no_features; i++)
    {
        indices[i] = 0;
    }
    //choose first random initial mean
    int random = (rand() % image_features.size());
    //int random = 17;
    if(indices[random]==0)
    {
        indices[random] = 1;
        cluster_means.push_back(image_features[random]); 
        cluster_indexes.push_back(random);
        //std::cout << random << " " << std::endl;
        std::cout << "initial cluser mean " << ": " << image_names[random] << std::endl; 
    }
    //std::vector<float> distances;
    int max = 0;
    int new_centroid = -1;
   

     //std::vector<float> distances;
    for(int k = 0; k < (no_clusters-1); k++)
    {
        float max = 0;
        for (int i = 0; i < image_features.size(); i++)
        {
            float min_distance = 10000000;
            if(indices[i]==0) //hasn't been chosen as centroid yet
            {
                for (int j = 0; j < cluster_means.size(); j++)
                {
                    //std::cout << "CHECKING CLUSTER MEAN " << j << std::endl;
                    //std::cout << "Getting distance for image " << image_names[i] << "cluster mean " << image_names[cluster_indexes[j]] << std::endl;
                    float distance = get_abs_distance(i, j, hist_size); //let cluster index be only the first element 0 for now
                    //float distance = get_euclid_distance(i, j, hist_size); 
                    //distances.push_back(distance);
                    //std::cout << distance << std::endl;
                    if(distance < min_distance)
                    {
                        min_distance = distance;
                        //std::cout << "new min" << min_distance << std::endl;
                    }
                    //std::cout << distance << std::endl;
                 
                }
                //std::cout << "Final min " << min_distance << " " << image_names[i] << std::endl;
                if(min_distance > max)
                {
                    max = min_distance;
                    new_centroid = i; //furthest distance
                    //std::cout << "new max" << max << " " << image_names[new_centroid] << std::endl;
                }
            }

        }
        indices[new_centroid] = 1; //mark that this has been used
        cluster_means.push_back(image_features[new_centroid]); 
        cluster_indexes.push_back(new_centroid);
        //std::cout << new_centroid << "with max distance: " << max << std::endl;
        //std::cout << image_names[new_centroid] << std::endl;
    }

    std::cout << cluster_means.size() << std::endl;
    std::cout << "INITIAL CLUSTER MEANS" << std::endl;
    for (int i  = 0 ; i < cluster_means.size(); i++)
    {
        std::cout << image_names[cluster_indexes[i]] << std::endl;
    }

}






//Commonly used initialization methods are Forgy and Random Partition
// The Forgy method randomly chooses k observations from the dataset and uses these as the initial means. 
void Clusterer::get_random_means(const int no_clusters)
{
    
    std::cout << "Get initial cluster means for " << no_clusters << " clusters" << std::endl;
    //int seed = 0; //or use clock to set this
    //use a seed that gives you good results
    //somehow make sure the random means are distributed !!
    //srand(time(NULL));
    srand(1000); //quite a good choice
    //std::vector<int> indices;
    int no_features = image_features.size();
     //std::cout << "No.features" << no_features << std::endl;
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
    //std::cout << "cluster size " <<  cluster_means.size() << std::endl;
    //check resulting cluster means
    std::cout << "INITIAL CLUSTER MEANS" << std::endl;
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


//is this euclidean distance betwene each bin between the image hist and a certain mean?
//OR should we use normalised euclidean distance?
//sometimes get nan??
//float Clusterer::get_euclid_distance(int hist1[], int hist2[], int hist_size)
float Clusterer::get_abs_distance(const int image_index, const int cluster_index, const int hist_size) const
{
    
    float sum = 0;
    //std::cout << "Hist_size" << hist_size << std::endl;
     //std::cout << "hist1 " << image_names[image_index] << " hist2 " << image_names[cluster_indexes[cluster_index]] << std::endl;
    for (int i = 0; i < hist_size; i++)
    {
        //sum += std::pow((image_features[image_index][i] - cluster_means[cluster_index][i]), 2); 
        sum += std::pow(std::abs((image_features[image_index][i] - cluster_means[cluster_index][i])), 1); 
        //sum += std::pow((image_features[24][i] - image_features[29][i]), 2);
        //std::cout << i << std::endl;
    }
    //sum = std::sqrt(sum); //NB - PUT BACK IN
    //std::cout << sum << std::endl;
    return sum;

}


float Clusterer::get_euclid_distance(const int image_index, const int cluster_index, const int hist_size) const
{
    
    float sum = 0;
    //std::cout << "Hist_size" << hist_size << std::endl;
     //std::cout << "hist1 " << image_names[image_index] << " hist2 " << image_names[cluster_indexes[cluster_index]] << std::endl;
    for (int i = 0; i < hist_size; i++)
    {
        //sum += std::pow((image_features[image_index][i] - cluster_means[cluster_index][i]), 2); 
        sum += std::pow((image_features[image_index][i] - cluster_means[cluster_index][i]), 2); 
        //sum += std::pow((image_features[24][i] - image_features[29][i]), 2);
        //std::cout << i << std::endl;
    }
    sum = std::sqrt(sum); //NB - PUT BACK IN OR TAKE OUT?
    //std::cout << sum << std::endl;
    return sum;

}




//Assign each observation to the cluster with the nearest mean: that with the least squared Euclidean distance
//should we allow for images to be different sizes?
void Clusterer::assign_to_cluster(const int hist_size)
{
    std::cout << "Assigning to clusters... " << std::endl;
    cluster_map.clear(); //refresh map
    int num_clusters = cluster_means.size();
    //int hist_size = std::ceil(double(size)/bin_size);
    //int hist_size = 255/bin_size; //NB to check this??
    //int hist [hist_size];
    //iterate over images and assign each to a cluster
    //for(int i = 0; i < greyscale_images.size(); i++)
    for(int i = 0; i < image_features.size(); i++)
    {
        //int * hist_ptr = create_histogram(i, 255, bin_size, size); //where to get maxVal from??
        //int * hist_ptr = image_features[i];
        //std::cout << "NEW IMAGE" << std::endl;
        float min = 100000000000; //initialise to very large number - make this a constant?
        int closest_cluster = -1; //set to -1 - indicates no closest cluster found
        for (int j=0; j < num_clusters; j++)
        {
            //std::cout << "Getting distance for image " << i << "cluster mean " << j << std::endl;
            float distance = get_euclid_distance(i, j, hist_size);
            //std::cout << "Distance: " << distance << std::endl;

            if(distance < min)
            {
                min = distance;
                //std::cout << "Min Distance: " << distance << std::endl;
                  //std::cout << "Cluster: " << j << std::endl;
                closest_cluster = j;
            }
        }
        //std::cout << "Closest Cluster " << closest_cluster << std::endl;
        cluster_map[closest_cluster].push_back(i); //assign index of greyscale image to that cluster
    }
    
}


//Recalculate means (centroids) for observations assigned to each cluster
//Assume mean is mean of each bin? = binwise average
void Clusterer::update_means(const int hist_size)
{
    std::cout << "Updating means..." << std::endl;
    has_converged = true; //must check for every cluster 
    //iterate through clusters
    for (int i = 0; i < cluster_means.size(); i++)
    {
        bool has_changed = update_bin_avgs(i, hist_size);
        if(has_changed == true)
        {
            has_converged = false;
        }
    }

}



 //the centroid is the point that minimizes the average squared Euclidean distance to the points in its cell?? - does this change things??
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
         ///* std::cout << "new: " << bin_avg << " old: " << old_bin_avgs[i];
        if(old_bin_avgs[i] != bin_avg) //if average has changed, the algorithm has not converged
        {
            has_changed = true;
        }
        
    }
    cluster_means[cluster_index] = new_bin_avgs;
    }
    return has_changed;
    
}


 //The algorithm has converged when the assignments no longer change.
 //or check when averages are no longer updated - may be easier
 //or make this into a boolean flag that is set during update_means()


//Call assign to cluster and update means iteratively until no further changes observed
//The algorithm has converged when the assignments no longer change.
void Clusterer::k_means(const int no_clusters, const int hist_size)
{
    //initialise cluster means
    //get_random_means(no_clusters);
    k_meansplusplus_init(no_clusters, hist_size);
    has_converged = false;
    //while averages are still changing
    std::cout << has_converged << std::endl;
    while(!(has_converged))
    {
        assign_to_cluster(hist_size); //assign each image to a cluster
        update_means(hist_size); //update cluster means based on new assignment
          std::cout << has_converged << std::endl;

    }
    
}


//friend operator
std::ostream & BLLSAM009::operator<<(std::ostream & os, const Clusterer & kt)
{
    for (auto const& x : kt.cluster_map)
    {
        os << "Cluster " << x.first << ": " << std::endl;
        //std::cout << "Images " << std::endl;
        //std::cout << "image" << x.second[0]; //as specified in doc
        //os << "image" << x.second[0] << " " << kt.image_names[x.second[0]]; 
        os << kt.image_names[x.second[0]]; 
        for(int i = 1; i < x.second.size(); i++)
        {
            //std::cout << ", image" << x.second[i]; //as specified in doc
            //os << ", \nimage" << x.second[i] << " " << kt.image_names[x.second[i]]; //for testing
            os << ", \n" + kt.image_names[x.second[i]]; //for testing
        }
        os << "\n" << std::endl;
    }
    return os;

}













