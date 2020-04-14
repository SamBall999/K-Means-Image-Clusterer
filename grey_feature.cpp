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
#include "grey_feature.h"
 


//NB! Check that memory is managed correctly!!

using namespace BLLSAM009;


Grey_Feature::Grey_Feature() // default constructor 
{
    
}


Grey_Feature::~Grey_Feature()
{
    //clean everything properly OR use special pointers
}



// populate the object with images 
int Grey_Feature::read_images(const std::string & folder_name)
{
    int num_images = 10;//how to set this?? POSIX?? - set back to 20
    int size;
    std::string prefixes[] = {"eight", "five", "four", "nine", "one", "seven", "six", "three", "two", "zero"};
    images.clear(); //ensure image vector is empty
    for (int k = 0; k < 10; k++)
    {
        for(int i = 1; i <= num_images; i++)
        {
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




int Grey_Feature::read_image(const std::string & image_name)
{
    //std::cout << "Reading " << image_name << std::endl;
    
    std::ifstream byte_file;
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
    //int max_val;
    
    std::istringstream iss(line);
    iss >> width >> std::ws >> height >> std::ws; //check newlines?
    byte_file >> MAX_VAL >> std::ws;


    

    int no_elements = height*width*3;
    char * byte_data = new char[no_elements]; //NB to clean up in destructor
    //std::unique_ptr<char []> byte_data(new char[size]);
    byte_file.read(byte_data, no_elements);

    unsigned char * image = (unsigned char *)byte_data;
    images.push_back(image);
    byte_file.close();


    //clean up later?? - make into unique pointers later
    //delete [] byte_data; //clean up allocated memory

    int size = width*height; //return size of greyscale image to be used in future
    return size;

    
}








void Grey_Feature::write_to_output()
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


//cycle through input images and convert each to greyscale
void Grey_Feature::convert_to_grey(int size)
{
     greyscale_images.clear();
    for(int i = 0; i < images.size(); i++)
    {
        to_greyscale(i, size);
    }
     write_grey_to_output();
}



//given an input image, convert each pixel to greyscale value
void Grey_Feature::to_greyscale(int index, int size)
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



//convert given R, G, B values to single greyscale value
unsigned char Grey_Feature::colour_conversion(unsigned char R, unsigned char G, unsigned char B)
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




//write greyscale image to output to validate conversion
void Grey_Feature::write_grey_to_output()
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



//NB this assumes size of image is constant
int Grey_Feature::get_image_features(int bin_size, int size)
{
     std::cout << "No. of greyscale images " << greyscale_images.size()  << std::endl;
    //image_features (greyscale_images);
    int hist_size = std::ceil((MAX_VAL+1)/(float)bin_size); //change back if necessary
    for(int i = 0; i < greyscale_images.size(); i++)
    {
        create_histogram(i, hist_size, bin_size, size); 
    }

    //int hist_size = 255/bin_size;
    for(int i = 0; i < image_features.size(); i+=1)
    {   
        std::cout << "NEW IMAGE" << i <<  std::endl;
        for (int j = 0; j < hist_size; j++)
        {
            std::cout << image_features[i][j] << " ";
        }
    }
    return hist_size;
}



void Grey_Feature::create_histogram(int index, int hist_size, int bin, int size)
{
    
    //use grey_images 
    //int hist_size = maxVal/bin;
    //int hist_size = (maxVal+1)/bin; //change back if breaks
    int frequencies[MAX_VAL+1]; //initialise a zero array with positions 0-255
    for (int a = 0; a < (MAX_VAL+1); a++) 
    {
        frequencies[a] = 0;
    }

    for (int i = 0; i < (size); i++)
    {
        
        frequencies[greyscale_images[index][i]]++;
       
    }
     group_in_bins(frequencies, hist_size, bin, (MAX_VAL+1));

}




void Grey_Feature::group_in_bins(const int frequencies[], int hist_size, int bin_size, int size)
{
    //based on given bin size, group histogram array 
    //std::cout << "Grouping..." << std::endl;

     //std::unique_ptr<int[]> hist(new int[hist_size]);
    int * hist = new int[hist_size];
    //int new_size = std::ceil(double(size)/bin_size); add back if breaks

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
            if(i > (size-1)) //check for values exceeding max value
            {
                break;
            }
            hist[count] += frequencies[i];
        }

    }
    image_features.push_back(hist);

}


 std::vector<int *>  Grey_Feature::get_image_features()
 {
     return image_features;
 }


std::vector<std::string>  Grey_Feature::get_image_names()
{
    return image_names;
}