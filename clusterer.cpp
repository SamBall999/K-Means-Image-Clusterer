#include <vector>
#include <string>
#include <cmath>
#include <map>
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

    images.clear(); //ensure image vector is empty
    
    std::ifstream byte_file;
    std::string image_name = "./Gradient_Numbers_PPMS/eight_2.ppm";
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



    //TEST
    //int test [] = {0, 0, 0, 5, 4, 3, 7, 0, 0, 0, 10, 255};

    //colour conversion should return (0, 4, 1, 25)

     
    //unsigned char * image = new unsigned char[size];
    std::unique_ptr<unsigned char []> image(new unsigned char[no_elements]);
    for(int i = 0; i < (no_elements); ++i)
    {
        //std::cout << static_cast<unsigned char>(byte_data[i]) << std::endl;
        image[i] = static_cast<unsigned char>(byte_data[i]);
    }
    /*for(int i = 0; i < 12; ++i)
    {
        //std::cout << static_cast<unsigned char>(byte_data[i]) << std::endl;
        image[i] = static_cast<unsigned char>(test[i]);
        std::cout << int(image[i]) << std::endl;
    }*/
    images.push_back(image.get()); 
    byte_file.close();

    delete [] byte_data; //clean up allocated memory

    int size = width*height; //return size of greyscale image to be used in future

    return size;
    //return 4;

    
}


void Clusterer::to_greyscale(int index, int size)
{
        //loop through pixels in image and call colour_conversion  
        
        std::unique_ptr<unsigned char []> grey_image(new unsigned char[size]);
        int count = 0;
        for (int i = 0; i < size*3; i+=3)
        {
            grey_image[count] = colour_conversion(images[index][i], images[index][i+1], images[index][i+2]);
            count++;
        }
        greyscale_images.push_back(grey_image.get());
         for(int j = 0; j < (size); ++j)
        {
            std::cout << int(grey_image[j]) << std::endl;
        }

}


unsigned char Clusterer::colour_conversion(unsigned char R, unsigned char G, unsigned char B)
{
    // a single value per pixel, which ranges from 0 (black) to 255 (white)
    std::cout << "R" << int(R) << "G"<< int(G) << "B" << int(B) << std::endl;
    int pixel_out =  (0.21*R) + (0.72*G) + (0.07*B);
    //unsigned char pixel_out =  (0.21*R) + (0.72*G) + (0.07*B);
    std::cout << "PIXEL OUT " << std::endl;
    std::cout << pixel_out << std::endl;
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
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        //int * hist_ptr = create_histogram(i, 255, bin_size, size);
        image_features[i]=create_histogram(i, 255, bin_size, size); //indexing is v NB
    }
}



//as an extra - draw a pop up histogram??
//**WARNING PIXEL VALUES AREN'T ALWAYS THE SAME?? - may have fixed this
int * Clusterer::create_histogram(int index, int maxVal, int bin, int size)
{
    //use grey_images 
    int hist [maxVal/bin];
    int frequencies[maxVal+1]; //initialise a zero array with positions 0-255
    for (int a = 0; a < (maxVal+1); a++) 
    {
        frequencies[a] = 0;
    }

    for (int i = 0; i < (size); i++){

        frequencies[greyscale_images[index][i]]++;
       
    }
    /*int test [] = {0, 0, 5, 4, 3, 7, 0, 0, 0, 10, 255};
    for(int i = 0; i < 11; i++){

        frequencies[test[i]]++;
       
    }*/
    //group_in_bins(frequencies, bin, maxVal);
    for (int k  = 0; k < (maxVal+1); k++)
    {
        std::cout << "Pixel value " << k << " Count  " << frequencies[k] << std::endl;
    }
     group_in_bins(frequencies, hist, 3, (maxVal+1));
     //std::unique_ptr<int[]> hist_ptr(hist);
     return hist;

}


void Clusterer::group_in_bins(const int frequencies[], int hist[], int bin_size, int size)
{
    //based on given bin size, group histogram array 
    std::cout << "Grouping..." << std::endl;

    int new_size = std::ceil(double(size)/bin_size);
    int hist [new_size]; //initialise binned histogram
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
        std::cout << "Index: " << k << " Count " << hist[k] << std::endl;
    }

}


//is this euclidean distance betwene each bin between the image hist and a certain mean?
//OR should we use normalised euclidean distance?
float Clusterer::get_euclid_distance(int hist1[], int hist2[], int hist_size)
{
    
    float sum;
    for (int i = 0; i < hist_size; i++)
    {
        sum += (hist1[i] - hist2[i])^2;
    }
    sum = std::sqrt(sum);
    return sum;

}


void Clusterer::get_random_means(int num_clusters)
{

}



//Assign each observation to the cluster with the nearest mean: that with the least squared Euclidean distance
//should we allow for images to be different sizes?
void Clusterer::assign_to_cluster(int size, int bin_size, int num_clusters)
{
    int new_size = std::ceil(double(size)/bin_size);
    int hist [new_size];
    //iterate over images and assign each to a cluster
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        //int * hist_ptr = create_histogram(i, 255, bin_size, size); //where to get maxVal from??
        int * hist_ptr = image_features[i];
        float min = 10000000000; //initialise to very large number
        int closest_cluster;
        for (int j; j < num_clusters; j++)
        {
            float distance = get_euclid_distance(hist_ptr, cluster_means[j], new_size);
            if(distance < min)
            {
                min = distance;
                closest_cluster = j;
            }
        }
        cluster_map[closest_cluster].push_back(i); //assign index of greyscale image to that cluster
    }
    
}


//Recalculate means (centroids) for observations assigned to each cluster
//Assume mean is mean of each bin? = binwise average
void Clusterer::update_means(int hist_size)
{
    for (int i = 0; i < cluster_means.size(); i++)
    {
        cluster_means[i] = get_bin_avgs(i, hist_size);
    }

    
}


void Clusterer::get_cluster_avg(int cluster_index, int hist_size)
{
    for(int i = 0; i < hist_size; i++)
    {
        
    }
}


//computes new bin averages for a single cluster mean
int* Clusterer::get_bin_avgs(int cluster_index, int hist_size)
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
    return new_bin_avgs;
    
}


//Call assign to cluster and update means iteratively until no further changes observed
void Clusterer::k_means()
{
    
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
}*/







//get keys in map
/*std::vector<unsigned int> Clusterer::extract_keys(std::map<unsigned int, int> const& map) 
{
  std::vector<unsigned int> keys;
  for (auto const& element : map) 
  {
    keys.push_back(element.first);
  }
  return keys;
}*/


