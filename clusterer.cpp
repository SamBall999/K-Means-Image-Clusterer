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


Clusterer::~Clusterer()
{
    //clean everything properly OR use special pointers
}






// populate the object with images 
int Clusterer::read_images(const std::string & folder_name)
{
    int num_images = 10;//how to set this?? POSIX?? - set back to 20
    int size;
    std::string prefixes[] = {"eight", "five", "four", "nine", "one", "seven", "six", "three", "two", "zero"};
    images.clear(); //ensure image vector is empty
    for (int k = 0; k < 10; k++)
    {
        for(int i = 1; i <= num_images; i++)
        {
            //std::cout << "Gradient_Numbers_PPMS/" + prefixes[k] + "_" + std::to_string(i) + ".ppm" << std::endl;
            std::string image_name = "Gradient_Numbers_PPMS/" + prefixes[k] + "_" + std::to_string(i) + ".ppm";
            size  = read_image(image_name);
            std::string display_name = prefixes[k] + "_" + std::to_string(i) + ".ppm";
            image_names.push_back(display_name);
        }
    }
    std::cout << "No of images read " << images.size() << std::endl;
    write_to_output();
    return size; //change this later
}



void Clusterer::write_to_output()
{
     
     if (std::remove("out1.ppm") != 0)
     {
		std::cout << "File deletion failed" << std::endl;
     }
     
     std::ofstream out_file;
     out_file.open("out1.ppm", std::ios::binary); 
     out_file << "P6" << std::endl;
     out_file << "32 32" << std::endl;
     out_file << "255" << std::endl;
     char byte_data[32*32*3];
     for (int i = 0; i < (32*32*3); i++)
     {
         byte_data[i] = (char)images[0][i];
     }
     out_file.write(byte_data, (32*32*3));


     if (std::remove("out2.ppm") != 0)
     {
		std::cout << "File deletion failed" << std::endl;
     }
     
     std::ofstream out_file2;
     out_file2.open("out2.ppm", std::ios::binary); 
     out_file2 << "P6" << std::endl;
     out_file2 << "32 32" << std::endl;
     out_file2 << "255" << std::endl;
    char byte_data2[32*32*3];
     for (int i = 0; i < (32*32*3); i++)
     {
         byte_data2[i] = (char)images[1][i];
     }
     out_file2.write(byte_data2, (32*32*3));

}



void Clusterer::write_grey_to_output()
{
     
     if (std::remove("out_grey1.ppm") != 0)
     {
		std::cout << "File deletion failed" << std::endl;
     }
     
     std::ofstream out_file;
     out_file.open("out_grey1.ppm", std::ios::binary); 
     out_file << "P6" << std::endl;
     out_file << "32 32" << std::endl;
     out_file << "255" << std::endl;
     char byte_data[32*32*3];
     int count = 0;
     for (int i = 0; i < (32*32*3); i+=3)
     {
         byte_data[i] = (char)greyscale_images[0][count];
         byte_data[i+1] = (char)greyscale_images[0][count];
         byte_data[i+2] = (char)greyscale_images[0][count];
         count++;
     }
     out_file.write(byte_data, (32*32*3));


     if (std::remove("out_grey2.ppm") != 0)
     {
		std::cout << "File deletion failed" << std::endl;
     }
     
     std::ofstream out_file2;
     out_file2.open("out_grey2.ppm", std::ios::binary); 
     out_file2 << "P6" << std::endl;
     out_file2 << "32 32" << std::endl;
     out_file2 << "255" << std::endl;
    char byte_data2[32*32*3];
    count = 0;
     for (int i = 0; i < (32*32*3); i+=3)
     {
         byte_data2[i] = (char)greyscale_images[1][count];
         byte_data2[i+1] = (char)greyscale_images[1][count];
         byte_data2[i+2] = (char)greyscale_images[1][count];
         count++;
     }
     out_file2.write(byte_data2, (32*32*3));

}



int Clusterer::read_image(const std::string & image_name)
{
    //std::cout << "Reading " << image_name << std::endl;
    
    std::ifstream byte_file;
    //std::string image_name = "./Gradient_Numbers_PPMS/eight_2.ppm";
    byte_file.open(image_name, std::ios::binary); 
    if (!byte_file)
    { 
        std::cout << "Error opening image file" <<std::endl; 
        return false; //return false if problem reading file
    }
    std::string file_id;
    byte_file >> file_id >> std::ws;
    if(file_id != "P6")
    {
        std::cout << "Wrong file format" <<std::endl; 
        return false; //return false if problem reading file
    }
    //read and discard comment lines - check for #
    std::string line;
    getline(byte_file, line, '\n');
    while(line[0] == '#')//check for comment
    {
        std::cout << "Discarding comment" << std::endl;
        getline(byte_file, line);
    }
    int width;
    int height;
    int max_val;
    
    std::istringstream iss(line);
    iss >> width >> std::ws >> height >> std::ws; //check newlines?
    byte_file >> max_val >> std::ws;

    //std::cout << width << std::endl;
    //std::cout << height << std::endl;
    //std::cout << max_val << std::endl;

    

    int no_elements = height*width*3;
    char * byte_data = new char[no_elements]; //NB to clean up in destructor
    //std::unique_ptr<char []> byte_data(new char[size]);
    byte_file.read(byte_data, no_elements);

    //std::unique_ptr<unsigned char []> image(new unsigned char[no_elements]);
    //PERHAPS:   std::unique_ptr<unsigned char []> image((unsigned char *)byte_data);
    //std::unique_ptr<unsigned char []> image((unsigned char *)byte_data);
    //PERHAPS: images.push_back(std::move(image));
    //images.push_back(std::move(image));
    // std::cout << "NEW IMAGE " << image_name << std::endl;
    /*for(int i = 0; i < (no_elements); ++i)
    {
        //std::cout <<  static_cast<int>(byte_data[i]) << " ";
        image[i] = static_cast<unsigned char>(byte_data[i]);
        //std::cout <<  static_cast<int>(image[i]) << " ";
    }*/
    //images.push_back(image.get()); 
    unsigned char * image = (unsigned char *)byte_data;
    images.push_back(image);
    byte_file.close();


    //clean up later?? - make into unique pointers later
    //delete [] byte_data; //clean up allocated memory

    int size = width*height; //return size of greyscale image to be used in future
    return size;

    
}








void Clusterer::convert_to_grey(int size)
{
     greyscale_images.clear();
    for(int i = 0; i < images.size(); i++)
    {
        to_greyscale(i, size);
    }
     write_grey_to_output();
}



void Clusterer::split_into_RGB(int size)
{
     colour_images.clear();
     unsigned char * R_values = new unsigned char[size];
     unsigned char * G_values = new unsigned char[size];
    unsigned char * B_values = new unsigned char[size];
    int count = 0;
    for(int i = 0; i < images.size(); i++)
    {
        for (int j = 0; j < size*3; j+=3)
        {
            R_values[count] = images[i][j];
            G_values[count] = images[i][j+1];
            B_values[count] = images[i][j+2];
            count++;
        }
        colour_images.push_back(R_values);
        colour_images.push_back(G_values);
        colour_images.push_back(B_values);
    }
}





void Clusterer::to_greyscale(int index, int size)
{
        //loop through pixels in image and call colour_conversion  
        
        //std::unique_ptr<unsigned char []> grey_image(new unsigned char[size]);
        unsigned char * grey_image = new unsigned char[size];
        int count = 0;
        for (int i = 0; i < size*3; i+=3)
        {
            grey_image[count] = colour_conversion(images[index][i], images[index][i+1], images[index][i+2]);
            count++;
        }
        //greyscale_images.push_back(grey_image.get());
        greyscale_images.push_back(grey_image);

}


unsigned char Clusterer::colour_conversion(unsigned char R, unsigned char G, unsigned char B)
{
    // a single value per pixel, which ranges from 0 (black) to 255 (white)
    //std::cout << "R" << int(R) << "G"<< int(G) << "B" << int(B) << std::endl;
    int pixel_out =  (0.21*R) + (0.72*G) + (0.07*B);
    //unsigned char pixel_out =  (0.21*R) + (0.72*G) + (0.07*B);
    //test range
    if(pixel_out < 0 )
    {
        std::cout << "Too low" << std::endl;
    }
    if(pixel_out > 255)
    {
        std::cout << "Too high" << std::endl;
    }
    return pixel_out;
}


//NB this assumes size of image is constant
void Clusterer::get_image_features(int bin_size, int size)
{
     std::cout << "No. of greyscale images " << greyscale_images.size()  << std::endl;
    //image_features (greyscale_images);
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        create_histogram(i, 255, bin_size, size); 
    }
}



//as an extra - draw a pop up histogram??
void Clusterer::create_histogram(int index, int maxVal, int bin, int size)
{
    //use grey_images 
    int hist_size = maxVal/bin;
    int frequencies[maxVal+1]; //initialise a zero array with positions 0-255
    for (int a = 0; a < (maxVal+1); a++) 
    {
        frequencies[a] = 0;
    }

    for (int i = 0; i < (size); i++){

        frequencies[greyscale_images[index][i]]++;
       
    }
     group_in_bins(frequencies, hist_size, bin, (maxVal+1));
    //group_in_bins(frequencies, hist_size, 3, (maxVal+1));
     /*for(int j = 0; j < 85; j++)
      {
            std::cout << image_features[index][j] << std::endl;
      }*/

}


//void Clusterer::group_in_bins(const int frequencies[], int hist[], int bin_size, int size)
void Clusterer::group_in_bins(const int frequencies[], int hist_size, int bin_size, int size)
{
    //based on given bin size, group histogram array 
    //std::cout << "Grouping..." << std::endl;

     //std::unique_ptr<int[]> hist(new int[hist_size]);
    int * hist = new int[hist_size];
    int new_size = std::ceil(double(size)/bin_size);
     for (int a = 0; a < new_size; a++) 
    {
        hist[a] = 0;
    }

     for(int count = 0; count < new_size; count++)
    {
         //std::cout << count*bin_size << std::endl;
        for (int i = count*bin_size; i < (count+1)*bin_size; i++)
        {
            if(i > (size-1)) //check for values exceeding max value
            {
                break;
            }
            hist[count] += frequencies[i];
        }

    }
    //image_features.push_back(hist.get());
    image_features.push_back(hist);

}






//Commonly used initialization methods are Forgy and Random Partition
// The Forgy method randomly chooses k observations from the dataset and uses these as the initial means. 
void Clusterer::get_random_means(int no_clusters)
{
    
    std::cout << "Get initial cluster means for " << no_clusters << " clusters" << std::endl;
    //int seed = 0; //or use clock to set this
    //use a seed that gives you good results
    //somehow make sure the random means are distributed !!
    srand(time(NULL));
    //std::vector<int> indices;
    int no_features = image_features.size();
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
    std::cout << "cluster size " <<  cluster_means.size() << std::endl;
    //check resulting cluster means
    /*std::cout << "INITIAL CLUSTER MEANS" << std::endl;
    for (int i  = 0 ; i < cluster_means.size(); i++)
    {
        for(int j =0; j < 85; j++)
        {
            std::cout << cluster_means[i][j] << ", ";
        }
        std::cout << "\n";
    }*/


}


//is this euclidean distance betwene each bin between the image hist and a certain mean?
//OR should we use normalised euclidean distance?
//sometimes get nan??
//float Clusterer::get_euclid_distance(int hist1[], int hist2[], int hist_size)
float Clusterer::get_euclid_distance(int image_index, int cluster_index, int hist_size)
{
    
    float sum = 0;
    for (int i = 0; i < hist_size; i++)
    {
         //std::cout << "hist1 " << image_features[image_index][i] << " hist2 " << cluster_means[cluster_index][i] << std::endl;
        sum += std::pow((image_features[image_index][i] - cluster_means[cluster_index][i]), 2);
        //std::cout << i << std::endl;
    }
    sum = std::sqrt(sum);
    return sum;

}



//Assign each observation to the cluster with the nearest mean: that with the least squared Euclidean distance
//should we allow for images to be different sizes?
void Clusterer::assign_to_cluster(int hist_size)
{
    std::cout << "Assigning to clusters... " << std::endl;
    cluster_map.clear(); //refresh map
    int num_clusters = cluster_means.size();
    //int hist_size = std::ceil(double(size)/bin_size);
    //int hist_size = 255/bin_size; //NB to check this??
    //int hist [hist_size];
    //iterate over images and assign each to a cluster
    for(int i = 0; i < greyscale_images.size(); i++)
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
void Clusterer::update_means(int hist_size)
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
bool Clusterer::update_bin_avgs(int cluster_index, int hist_size)
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
void Clusterer::k_means(int no_clusters, int hist_size)
{
    //initialise cluster means
    get_random_means(no_clusters);
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
        //std::cout << "Images " << std::endl;
        //std::cout << "image" << x.second[0]; //as specified in doc
         os << "image" << x.second[0] << " " << kt.image_names[x.second[0]]; 
        for(int i = 1; i < x.second.size(); i++)
        {
            //std::cout << ", image" << x.second[i]; //as specified in doc
            os << ", \nimage" << x.second[i] << " " << kt.image_names[x.second[i]]; //for testing
        }
        os << "\n" << std::endl;
    }
    return os;

}




//get values in map
/*std::vector<int> Clusterer::extract_values(std::map<unsigned int, int> const& map) 
{
  std::vector<int> values;
  for (auto const& element : map)
  {
    values.push_back(element.second);
  }
  return values;
}







//get keys in map
std::vector<unsigned int> Clusterer::extract_keys(std::map<unsigned int, int> const& map) 
{
  std::vector<unsigned int> keys;
  for (auto const& element : map) 
  {
    keys.push_back(element.first);
  }
  return keys;
}*/


