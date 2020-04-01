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
    int num_images = 20;//how to set this?? POSIX?? - set back to 20
    int sizes[num_images];
    images.clear(); //ensure image vector is empty
    for(int i = 1; i <= num_images/2; i++)
    {
        std::cout << "Gradient_Numbers_PPMS/eight_" + std::to_string(i) + ".ppm" << std::endl;
        std::string image_name = "Gradient_Numbers_PPMS/eight_" + std::to_string(i) + ".ppm";
        sizes[i-1] = read_image(image_name);
    }
    for(int i = 1; i <= num_images/2; i++)
    {
        std::string image_name = "./Gradient_Numbers_PPMS/five_" +  std::to_string(i) + ".ppm";
        std::cout << image_name << std::endl;
        sizes[i] = read_image(image_name);
    }
    //std::string image_name = "./Gradient_Numbers_PPMS/five_" + std::to_string(4) + ".ppm";
    //sizes[0] = read_image(image_name);
    //check if all sizes are the same
    int check_size = sizes[0];
    for(int i = 1; i < num_images; i++)
    {
        if(sizes[i]!=check_size)
        {
            std::cout << "Error! Images have different sizes " << check_size << " " << sizes[i] << std::endl;
        }
    }
    std::cout << "No of images read " << images.size() << std::endl;
    write_to_output();
    return sizes[0]; //change this later
}



void Clusterer::write_to_output()
{
     
     //examine contents
     /*for(int i = 0; i < images.size(); i++)
     {
         std::cout << "\nimage " << i << std::endl;
         for(int j = 0; j < (32*32*3); j++)
         {
             std::cout << images[i][j] << " ";
         }
     }*/
     
     
     
     
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
    std::cout << "Reading " << image_name << std::endl;
    
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

    std::cout << width << std::endl;
    std::cout << height << std::endl;
    std::cout << max_val << std::endl;

    

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




void Clusterer::to_greyscale(int index, int size)
{
        //loop through pixels in image and call colour_conversion  
        
        //std::unique_ptr<unsigned char []> grey_image(new unsigned char[size]);
        unsigned char * grey_image = new unsigned char[size];
        int count = 0;
        std::cout << "NEW IMAGE " << index << std::endl;
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
        //int * hist_ptr = create_histogram(i, 255, bin_size, size);
        //image_features[i]=create_histogram(i, 255, bin_size, size); //indexing is v NB
         std::cout << "Image  " << i << " " << int(greyscale_images[i][2])  << std::endl;
        create_histogram(i, 255, bin_size, size); 
    }
}



//as an extra - draw a pop up histogram??
//**WARNING PIXEL VALUES AREN'T ALWAYS THE SAME?? - may have fixed this
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
    /*for (int k  = 0; k < (maxVal+1); k++)
    {
        std::cout << "Pixel value " << k << " Count  " << frequencies[k] << std::endl;
    }*/
     group_in_bins(frequencies, hist_size, 3, (maxVal+1));
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
    //int hist [new_size]; //initialise binned histogram
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
    for (int k  = 0; k < (new_size); k++)
    {
        //std::cout << "Index: " << k << " Count " << hist[k] << std::endl;
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
            i++;
        }
    }
    std::cout << "cluster size " <<  cluster_means.size() << std::endl;
    //check resulting cluster means
    /*for (int i  = 0 ; i < cluster_means.size(); i++)
    {
        std::cout << cluster_means[i][0] << std::endl;
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
void Clusterer::assign_to_cluster(int size, int bin_size)
{
    int num_clusters = cluster_means.size();
    int hist_size = std::ceil(double(size)/bin_size);
    int hist [hist_size];
    std::cout << "cluster size " <<  cluster_means.size() << std::endl;
    //iterate over images and assign each to a cluster
    std::cout << "hist_size " <<  hist_size << std::endl;
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        //int * hist_ptr = create_histogram(i, 255, bin_size, size); //where to get maxVal from??
        //int * hist_ptr = image_features[i];
        float min = 100000000000; //initialise to very large number
        int closest_cluster = -1; //set to -1 - indicates no closest cluster found
        for (int j=0; j < num_clusters; j++)
        {
            //std::cout << "Getting distance for image " << i << "cluster mean " << j << std::endl;
            float distance = get_euclid_distance(i, j, hist_size);
            //std::cout << "Distance: " << distance << std::endl;

            if(distance < min)
            {
                min = distance;
                closest_cluster = j;
            }
        }
        std::cout << "Closest Cluster " << closest_cluster << std::endl;
        cluster_map[closest_cluster].push_back(i); //assign index of greyscale image to that cluster
    }
    for (auto const& x : cluster_map)
    {
        std::cout << "Cluster" << x.first << std::endl;
        std::cout << "Images " << std::endl;
        for(int i = 0; i < x.second.size(); i++)
        {
            std::cout << x.second[i] << " ";
        }
         std::cout << "\n" << std::endl;
    }
    
}


//Recalculate means (centroids) for observations assigned to each cluster
//Assume mean is mean of each bin? = binwise average
void Clusterer::update_means(int size, int bin_size)
{
    int hist_size = std::ceil(double(size)/bin_size);
    for (int i = 0; i < cluster_means.size(); i++)
    {
        //cluster_means[i] = get_bin_avgs(i, hist_size);
        update_bin_avgs(i, hist_size);
    }

    
}


void Clusterer::get_cluster_avg(int cluster_index, int hist_size)
{
    for(int i = 0; i < hist_size; i++)
    {
        
    }
}



 //the centroid is the point that minimizes the average squared Euclidean distance to the points in its cell?? - does this change things??
//computes new bin averages for a single cluster mean
void Clusterer::update_bin_avgs(int cluster_index, int hist_size)
{
    //creat global variable holding histograms (image features) of each image
    
    //get pixels associated with the cluster index
    int new_bin_avgs[hist_size];
    std::vector<int> image_indexes = cluster_map.at(cluster_index);
    for(int i = 0; i < hist_size; i++) //for a certain bin
    {   
        float sum = 0;
        for(int j = 0; j < image_indexes.size(); j++) //average all values of that bin over all images in the cluster
        {
            sum += image_features[j][i]; //get ith bin of jth image feature
        }
        float bin_avg = sum/image_indexes.size();
        new_bin_avgs[i] = bin_avg;
        
    }
    cluster_means[cluster_index] = new_bin_avgs;
    //return new_bin_avgs;
    
}


 //The algorithm has converged when the assignments no longer change.
bool has_converged()
{

}


//Call assign to cluster and update means iteratively until no further changes observed
//The algorithm has converged when the assignments no longer change.
void Clusterer::k_means()
{
    /*while(set of clusters is still changing)
    {
        int hist_size = std::ceil(double(size)/bin_size);
        assign to cluster(hist_size);
        update_means(hist_size);

    }*/
    
}





//get values in map
std::vector<int> Clusterer::extract_values(std::map<unsigned int, int> const& map) 
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
}


