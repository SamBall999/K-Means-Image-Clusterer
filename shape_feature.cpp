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
#include "shape_feature.h"
 

// The idea for creating a shape encoding histogram using centre of mass was inspired by the following resource:
// https://www.cse.usf.edu/~r1k/MachineVisionBook/MachineVision.files/MachineVision_Chapter2.pdf

using namespace BLLSAM009;


Shape_Feature::Shape_Feature() 
{
    
}


Shape_Feature::~Shape_Feature()
{
   
}


//get all file names in given folder
std::string Shape_Feature::get_file_names(const std::string & folder_name) const {
   char buffer[128];
   std::string command = "ls " + folder_name;
   std::cout << "Reading images from " << folder_name << std::endl;
   std::string result = "";

   // open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed";
   }

   // read till end of process
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}


// populate the object with images 
void Shape_Feature::read_images(const std::string & folder_name) 
{
    int size;
    images.clear(); //ensure image vector is empty
    std::string filenames = get_file_names(folder_name);
   
    std::istringstream iss(filenames);
    std::string image_name;
    while(iss >> image_name)
    {
        std::string image_path = folder_name + "/" + image_name;
        read_image(image_path);
        std::size_t suffix_index = image_name.find(".");
        std::string display_name = image_name.substr(0,suffix_index);
        image_names.push_back(display_name);

    }

    std::cout << "No. of images read: " << images.size() << std::endl;
    
}



void Shape_Feature::read_image(const std::string & image_name)
{
    
    std::ifstream byte_file;
    byte_file.open(image_name, std::ios::binary); 
    if (!byte_file)
    { 
        std::cout << "Error opening image file" <<std::endl; //if problem reading file
    }
    std::string file_id;
    byte_file >> file_id >> std::ws;
    if(file_id != "P6")
    {
        std::cout << "Wrong file format" <<std::endl; //if wrong file format
    }
    //read and discard comment lines - check for #
    std::string line;
    getline(byte_file, line, '\n');
    while(line[0] == '#')//check for comment
    {
        std::cout << "Discarding comment" << std::endl;
        getline(byte_file, line);
    }
    
    std::istringstream iss(line);
    iss >> width >> std::ws >> height >> std::ws; //write to global variables for later use
    byte_file >> MAX_VAL >> std::ws;
    size = width*height; //store size of image to be used in future

    int no_elements = height*width*3;
    char * byte_data = new char[no_elements]; 
    byte_file.read(byte_data, no_elements);
    unsigned char * image = (unsigned char *)byte_data;
    images.push_back(image);
    byte_file.close();

    
}



//find centre of mass for given image
void Shape_Feature::calc_weighted_centre(const int index)
{
    int x_centre = 0;
    int y_centre = 0;
    float sum_of_masses = 0;
    float weighted_x = 0;
    float weighted_y = 0;
    for (int i = 0;  i < size; i++)
    {
        int x = i%width;
        int y = i/width; 
        if(colour_images[index][i]!=0)
        {
            /*weighted_x += x;
            weighted_y += y;
            sum_of_masses += 1;*/
            weighted_x += x*colour_images[index][i];
            weighted_y += y*colour_images[index][i];
            sum_of_masses += colour_images[index][i];
        }
    }

    x_centre = std::round(weighted_x/sum_of_masses);
    y_centre = std::round(weighted_y/sum_of_masses);
    x_centres.push_back(x_centre);
    y_centres.push_back(y_centre); 

}



//for each image, find centre of mass
void Shape_Feature::calculate_image_centres()
{
    process_colour_images();
    x_centres.clear();
    y_centres.clear();
    int count = 0;
    for(int i = 0; i < colour_images.size(); i++)
    {
        calc_weighted_centre(i);
    }
}



//calculate distances from each non-black pixel to the centre of mass
void Shape_Feature::calc_distances_from_centre(const int index)
{
    int * dist = new int[size];
    for(int i = 0; i < size; i++)
    {
        dist[i] = 0;
    }

    int x_dist;
    int y_dist;

    for(int i = 0; i < size; i++)
    {
        int x = i%width;
        int y = i/width; 
        //only for non-black pixels
        if(colour_images[index][i] != 0)
        {
            x_dist = x - x_centres[index]; 
            y_dist = y - y_centres[index];
            dist[i] = std::round(std::sqrt(std::pow(x_dist, 2) + std::pow(y_dist, 2))); //find combined distance
        }
    }
    distances.push_back(dist); //all distances for either an R, G or B element

}


//calculate distances to centre of mass for each image
void Shape_Feature::calc_distances()
{
    distances.clear();
    for(int i = 0; i < colour_images.size(); i++)
    {
        calc_distances_from_centre(i);
    }
}


//group into R, G and B arrays
void Shape_Feature::process_colour_images()
{
    colour_images.clear();
    int count = 0;
    for(int i = 0; i < images.size(); i++)
    {
        split_into_RGB(i);
    }
}



void Shape_Feature::split_into_RGB(const int index)
{
     unsigned char * R_values = new unsigned char[size];
     unsigned char * G_values = new unsigned char[size];
    unsigned char * B_values = new unsigned char[size];
    int count = 0;
    for (int j = 0; j < size*3; j+=3)
    {
        R_values[count] = images[index][j];
        G_values[count] = images[index][j+1];
        B_values[count] = images[index][j+2];
        count++;
    }
    colour_images.push_back(R_values);
    colour_images.push_back(G_values);
    colour_images.push_back(B_values);
   
}




//assumes size of image is constant
void Shape_Feature::calculate_image_features(const int bin_size)
{
    calc_distances();
    int largest_dist = 21; //distance to corner
    int opt_bin_size = OPTIMAL_BIN_WIDTH; //set this to bin_size if want to use user parameter
    hist_size = std::ceil((largest_dist+1)/(float)opt_bin_size); //use optimal bin width 
    for(int i = 0; i < colour_images.size(); i+=3)
    {
        create_histogram(i, opt_bin_size); 
        create_histogram(i+1, opt_bin_size); 
        create_histogram(i+2, opt_bin_size); 
    }
    //concatenate R, G and B histograms into one feature for each image
    combine_histograms();
}


//create histogram feature for the given image
void Shape_Feature::create_histogram(const int index, const int bin_size)
{
    int largest_dist = 21;
    int frequencies[largest_dist+1]; //initialise a zero array with positions 0-distance to corner
    for (int a = 0; a < (largest_dist+1); a++) 
    {
        frequencies[a] = 0;
    }

    for (int i = 0; i < (size); i++)
    {
    
        frequencies[distances[index][i]]++;
       
    }
    group_in_bins(frequencies, bin_size);

}


//create a binned histogram based on given bin size
void Shape_Feature::group_in_bins(const int frequencies[], const int bin_size)
{
    //based on given bin size, group histogram array 
    int * hist = new int[hist_size];
    int largest_dist = 21;
    //initialise binned histogram
     for (int a = 0; a < hist_size; a++) 
    {
        hist[a] = 0;
    }

    //group histograms values into bins
     for(int count = 0; count < hist_size; count++)
    {
        for (int i = count*bin_size; i < (count+1)*bin_size; i++)
        {
            if(i > (largest_dist)) //check for values exceeding max value
            {
                break;
            }
            hist[count] += frequencies[i];
        }

    }
    image_features.push_back(hist);

}



//combine R, G and B histograms into one combined feature
void Shape_Feature::combine_histograms()
{
    for(int i = 0; i < image_features.size(); i+=3)
    {   
       concat_arrays(i);
    }

}


//concatenate histogram arrays for the given image
void Shape_Feature::concat_arrays(const int index)
{
    int * result = new int[hist_size*3];
    std::copy(image_features[index], image_features[index] + hist_size, result);
    std::copy(image_features[index+1], image_features[index+1] + hist_size, result + hist_size);  
    std::copy(image_features[index+2], image_features[index+2] + hist_size, result + (hist_size*2));  
    combined_features.push_back(result);

}



 std::vector<int *>  Shape_Feature::get_image_features()
 {
     return combined_features;
 }


std::vector<std::string>  Shape_Feature::get_image_names()
{
    return image_names;
}

//allows clusterer to access the histogram size
int Shape_Feature::get_hist_size() 
{
    return hist_size*3;
}