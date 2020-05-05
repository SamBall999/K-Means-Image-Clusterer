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
#include "colour_feature.h"
 



using namespace BLLSAM009;


Colour_Feature::Colour_Feature() 
{
    
}


Colour_Feature::~Colour_Feature()
{
    
}


//get all file names in given folder
std::string Colour_Feature::get_file_names(const std::string & folder_name) const {
   char buffer[128];
   std::string command = "ls " + folder_name;
   std::cout << "Reading images from " << folder_name << std::endl;
   std::string result = "";

   // Open pipe to file
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
void Colour_Feature::read_images(const std::string & folder_name)
{
    int size;
    images.clear(); //ensure image vector is empty
    std::string filenames = get_file_names(folder_name); //get file names in folder
    std::istringstream iss(filenames);
    std::string image_name;
    while(iss >> image_name)
    {
        std::string image_path = "Gradient_Numbers_PPMS/" + image_name;
        read_image(image_path);
        std::size_t suffix_index = image_name.find(".");
        std::string display_name = image_name.substr(0,suffix_index);
        image_names.push_back(display_name);

    }

    std::cout << "No. of images read: " << images.size() << std::endl;

}




void Colour_Feature::read_image(const std::string & image_name)
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
    //read and discard comment lines
    std::string line;
    getline(byte_file, line, '\n');
    while(line[0] == '#')//check for comment
    {
        std::cout << "Discarding comment" << std::endl;
        getline(byte_file, line);
    }
    int width;
    int height;
    
    std::istringstream iss(line);
    iss >> width >> std::ws >> height >> std::ws; 
    byte_file >> MAX_VAL >> std::ws;
    

    int no_elements = height*width*3;
    char * byte_data = new char[no_elements]; 
    byte_file.read(byte_data, no_elements);
    unsigned char * image = (unsigned char *)byte_data;
    images.push_back(image);
    byte_file.close();

    size = width*height; //set size instance variable of greyscale image 

    
}



//process each colour image by grouping into R, G and B values
void Colour_Feature::process_colour_images()
{
    colour_images.clear();
    int count = 0;
    for(int i = 0; i < images.size(); i++)
    {
        split_into_RGB(i);
    }
}



//group pixel values into R, G and B for later processing
void Colour_Feature::split_into_RGB(const int index)
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


//create histogram feature for each colour image
void Colour_Feature::calculate_image_features(const int bin_size)
{
    process_colour_images();
    hist_size = std::ceil((MAX_VAL+1)/(float)bin_size); //calculate binned histogram size
    for(int i = 0; i < colour_images.size(); i+=3)
    {
        create_histogram(i, bin_size); //histogram of R pixels
        create_histogram(i+1, bin_size); //histogram of G pixels
        create_histogram(i+2, bin_size); //histogram of B pixels
    }
    //concatenate R, G and B histograms into one feature for each image
    combine_histograms();
}


//combine R, G and B histograms into one combined feature
void Colour_Feature::combine_histograms()
{
    for(int i = 0; i < image_features.size(); i+=3)
    {   
       concat_arrays(i);
    }
}


//concatenate histogram arrays for the given image
void Colour_Feature::concat_arrays(const int index)
{
    int * result = new int[hist_size*3];
    std::copy(image_features[index], image_features[index] + hist_size, result);
    std::copy(image_features[index+1], image_features[index+1] + hist_size, result + hist_size);  
    std::copy(image_features[index+2], image_features[index+2] + hist_size, result + (hist_size*2));  
    combined_features.push_back(result);

}



//create histogram feature for the given image
void Colour_Feature::create_histogram(const int base_index, const int bin)
{

    //initialise a zero array with positions 0-255
    int frequencies[MAX_VAL+1]; 
    for (int a = 0; a < (MAX_VAL+1); a++) 
    {
        frequencies[a] = 0;
    }

    for (int i = 0; i < (size); i++)
    {
    
        frequencies[colour_images[base_index][i]]++;
       
    }
     group_in_bins(frequencies, bin);

}


//create a binned histogram based on given bin size
void Colour_Feature::group_in_bins(const int frequencies[], const int bin_size)
{
    //based on given bin size, group histogram array 
    int * hist = new int[hist_size];

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
            if(i > MAX_VAL) //check for values exceeding max value
            {
                break;
            }
            hist[count] += frequencies[i];
        }

    }
    image_features.push_back(hist);

}

//allows clusterer to access the image features
 std::vector<int *> Colour_Feature::get_image_features()
 {
     //return image_features;
     return combined_features;
 }

//allows clusterer to access the image names
std::vector<std::string> Colour_Feature::get_image_names()
{
    return image_names;
}


//allows clusterer to access the histogram size
int Colour_Feature::get_hist_size() 
{
    return hist_size*3;
}