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
#include "two_d_feature.h"
 


// The idea for creating a two dimensional histogram was inspired by the following resource:
//https://www.pyimagesearch.com/2014/01/22/clever-girl-a-guide-to-utilizing-color-histograms-for-computer-vision-and-image-search-engines/

using namespace BLLSAM009;


Two_D_Feature::Two_D_Feature() 
{
    
}


Two_D_Feature::~Two_D_Feature()
{
   
}


//get all file names in given folder
std::string Two_D_Feature::get_file_names(const std::string & folder_name) const {
   char buffer[128];
   std::string command = "ls " + folder_name;
   std::cout << "Reading images from " << folder_name << std::endl;
   std::string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}


// populate the object with images 
void Two_D_Feature::read_images(const std::string & folder_name)
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




void Two_D_Feature::read_image(const std::string & image_name)
{
    
    std::ifstream byte_file;
    byte_file.open(image_name, std::ios::binary); 
    if (!byte_file)
    { 
        std::cout << "Error opening image file" <<std::endl;  //if problem reading file
    }
    std::string file_id;
    byte_file >> file_id >> std::ws;
    if(file_id != "P6")
    {
        std::cout << "Wrong file format" <<std::endl;  //if wrong file format
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

    size = width*height; 
  
}






void Two_D_Feature::process_colour_images()
{
    colour_images.clear();
    int count = 0;
    for(int i = 0; i < images.size(); i++)
    {
        split_into_RGB(i);
    }
}




void Two_D_Feature::split_into_RGB(const int index)
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
void Two_D_Feature::calculate_image_features(const int bin_size)
{
    process_colour_images(); //split into R, G and B arrays
    int opt_bin_size = OPTIMAL_BIN_WIDTH; //set this to bin_size if want to use user parameter
    hist_size = std::ceil((MAX_VAL+1)/(float)opt_bin_size); //use optimal bin size for good results
    for(int i = 0; i < colour_images.size(); i+=3)
    {
        create_2D_histogram(i, i+1, opt_bin_size); //R and B
        create_2D_histogram(i+1, i+2, opt_bin_size); //B and G
        create_2D_histogram(i+2, i, opt_bin_size);  //G and R
    }
    //concatenate R&B, G&B and B&R histograms into one feature for each image
    combine_histograms();

}



void Two_D_Feature::create_2D_histogram(const int axis_1, const int axis_2, const int bin)
{

    int ** frequencies = new int*[MAX_VAL+1]; 

    //initialise binned histogram
     for (int a = 0; a < (MAX_VAL+1); a++) 
    {
        frequencies[a] = new int[MAX_VAL+1];
        // Initialise the array with zeroes
        for(int b = 0; b < (MAX_VAL+1); b++)
        { 
            frequencies[a][b] = 0; 
        }   
    }


    //iterate through each pixel
    for (int i = 0; i < (size); i++)
    {
    
        //increment position in histogram corresponding to [R][B]
        frequencies[colour_images[axis_1][i]][colour_images[axis_2][i]]++;
       
    }

    group_in_bins(frequencies, bin);

}



void Two_D_Feature::group_in_bins(int ** frequencies, const int bin_size)
{
    //based on given bin size, group histogram array 
    int ** hist = new int*[hist_size]; 

    //initialise binned histogram
     for (int a = 0; a < hist_size; a++) 
    {
        hist[a] = new int[hist_size];
        // Initialise the array with zeroes
        for(int b = 0; b < hist_size; b++)
        { 
            hist[a][b] = 0; 
        }   
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
            for(int inner_count = 0; inner_count < hist_size; inner_count++)
            {
                for (int j = inner_count*bin_size; j < (inner_count+1)*bin_size; j++)
                {
                    if(j > MAX_VAL) //check for values exceeding max value
                    {
                        break;
                    }
                    hist[count][inner_count] += frequencies[i][j];
                }
            }
        }

    }
    image_features.push_back(hist);

}

void Two_D_Feature::combine_histograms()
{
    for(int i = 0; i < image_features.size(); i+=3)
    {   
       concat_arrays(i);
    }
}


void Two_D_Feature::concat_arrays(const int index)
{
    int * result = new int[hist_size*hist_size*3];

    //flatten each 2D array
    int * RB = new int[hist_size*hist_size];
    int * RG = new int[hist_size*hist_size];
    int * GB = new int[hist_size*hist_size];
    int count = 0;
    for (int i = 0; i < hist_size; i++)
    {
            for (int j = 0; j < hist_size; j++)
            {   
                RB[count] = image_features[index][i][j];
                RG[count] = image_features[index+1][i][j];
                GB[count] = image_features[index+2][i][j];
                count++;
            }
    }
    //concatenate the three flattened arrays
    std::copy(RB, RB + (hist_size*hist_size), result);
    std::copy(RG, RG + (hist_size*hist_size), result + (hist_size*hist_size));  
    std::copy(GB, GB + (hist_size*hist_size), result + ((hist_size*hist_size)*2));  
  
    combined_features.push_back(result);

}



 std::vector<int *> Two_D_Feature::get_image_features()
 {
     return combined_features;
 }


std::vector<std::string> Two_D_Feature::get_image_names()
{
    return image_names;
}


//allows clusterer to access the histogram size
int Two_D_Feature::get_hist_size() 
{
    return hist_size*hist_size*3; 
}