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
#include <cstdio>
#include "grey_feature.h"
 


using namespace BLLSAM009;


Grey_Feature::Grey_Feature() 
{
    
}



Grey_Feature::~Grey_Feature()
{

}






//get all file names in given folder
std::string Grey_Feature::get_file_names(const std::string & folder_name) const {
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
void Grey_Feature::read_images(const std::string & folder_name)
{
    int size;
    images.clear(); //ensure image vector is empty
    std::string filenames = get_file_names(folder_name); //get file names in folder
    std::istringstream iss(filenames);
    std::string image_name;

    //loop through each file name to read in image
    while(iss >> image_name)
    {
        std::string image_path = folder_name + "/" + image_name;
        read_image(image_path);
        std::size_t suffix_index = image_name.find(".");
        std::string display_name = image_name.substr(0,suffix_index); //clean filename for display
        image_names.push_back(display_name); 

    }

    std::cout << "No. of images read: " << images.size() << std::endl;
}



//read and store image data from given binary file
void Grey_Feature::read_image(const std::string & image_name)
{
    
    std::ifstream byte_file;
    //open binary file
    byte_file.open(image_name, std::ios::binary); 
    if (!byte_file)
    { 
        std::cout << "Error opening image file" <<std::endl;  //if problem reading file
    }
    //check file format
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
    iss >> width >> std::ws >> height >> std::ws; //read in width and height
    byte_file >> MAX_VAL >> std::ws; //read in maximum value
    size = width*height; //store size of greyscale image to be used in future

    //read remaining byte data
    int no_elements = height*width*3;
    char * byte_data = new char[no_elements]; 
    byte_file.read(byte_data, no_elements);
    unsigned char * image = (unsigned char *)byte_data;
    images.push_back(image); //add to vector
    byte_file.close();
    
}


//cycle through input images and convert each to greyscale
void Grey_Feature::convert_to_grey()
{
     greyscale_images.clear();
    for(int i = 0; i < images.size(); i++)
    {
        to_greyscale(i);
    }
}



//given an input image, convert each pixel to greyscale value
void Grey_Feature::to_greyscale(const int index)
{
        //loop through pixels in image and call colour_conversion  
        unsigned char * grey_image = new unsigned char[size];
        int count = 0;
        for (int i = 0; i < size*3; i+=3)
        {
            grey_image[count] = colour_conversion(images[index][i], images[index][i+1], images[index][i+2]);
            count++;
        }
        greyscale_images.push_back(grey_image);

}



//convert given R, G, B values to single greyscale value
unsigned char Grey_Feature::colour_conversion(const unsigned char R, const unsigned char G, const unsigned char B) const
{
    //returns a single value per pixel, which ranges from 0 (black) to 255 (white)
    int pixel_out =  (0.21*R) + (0.72*G) + (0.07*B);
    //check if inside desired range
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



//create histogram feature for each greyscale image
void Grey_Feature::get_image_features(const int bin_size)
{
    hist_size = std::ceil((MAX_VAL+1)/(float)bin_size); //calculate size of binned histogram
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        create_histogram(i, bin_size); 
    }
}


//create histogram feature for the given image
void Grey_Feature::create_histogram(const int index, const int bin)
{
    
    //initialise a zero array with positions 0-255
    int frequencies[MAX_VAL+1]; 
    for (int a = 0; a < (MAX_VAL+1); a++) 
    {
        frequencies[a] = 0;
    }
    //count the frequency of each pixel value
    for (int i = 0; i < (size); i++)
    {
        
        frequencies[greyscale_images[index][i]]++;
       
    }
    //group into bins of given bin size
     group_in_bins(frequencies, bin);

}



//create a binned histogram based on given bin size
void Grey_Feature::group_in_bins(const int frequencies[], const int bin_size)
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
    image_features.push_back(hist); //add to image features vector

}


//allows clusterer to access the image features
 std::vector<int *> Grey_Feature::get_image_features() 
 {
     return image_features;
 }


//allows clusterer to access the image names
std::vector<std::string> Grey_Feature::get_image_names() 
{
    return image_names;
}


//allows clusterer to access the histogram size
int Grey_Feature::get_hist_size() 
{
    return hist_size;
}