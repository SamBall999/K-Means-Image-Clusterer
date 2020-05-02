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
 


//NB! Check that memory is managed correctly!!
//Must remember to delete arrays or use special pointers

using namespace BLLSAM009;


Shape_Feature::Shape_Feature() // default constructor 
{
    
}


Shape_Feature::~Shape_Feature()
{
    //clean everything properly OR use special pointers
}

std::string Shape_Feature::get_file_names(const std::string & folder_name) const {
   char buffer[128];
   std::string command = "ls " + folder_name;
   std::cout << command << std::endl;
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
int Shape_Feature::read_images(const std::string & folder_name) 
{
    int size;
    images.clear(); //ensure image vector is empty
    std::string filenames = get_file_names(folder_name);
    //std::cout << filenames << std::endl;
    std::istringstream iss(filenames);
    std::string image_name;
    while(iss >> image_name)
    {
        //std::cout << image_name << std::endl;
        std::string image_path = "Gradient_Numbers_PPMS/" + image_name;
        size  = read_image(image_path);
        std::size_t suffix_index = image_name.find(".");
        std::string display_name = image_name.substr(0,suffix_index);
        image_names.push_back(display_name);

    }

    std::cout << "No of images read " << images.size() << std::endl;
    write_to_output();
    return size; //change this later
}




int Shape_Feature::read_image(const std::string & image_name)
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
    //int width;
    //int height;
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








void Shape_Feature::write_to_output() const
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




void Shape_Feature::get_weighted_centre(const int index, const int size)
{
    int x_centre = 0;
    int y_centre = 0;
    float sum_of_masses = 0;
    float weighted_x = 0;
    //float sum_of_y_masses = 0; //should only need one of these?
    float weighted_y = 0;
    for (int i = 0;  i < size; i++)
    {
        int x = i%width;
        //std::cout << x << std::endl;
        int y = i/width; 
        if(colour_images[index][i]!=0)
        {
            weighted_x += x;
            weighted_y += y;
            sum_of_masses += 1;
        }
    }
    //std::cout << weighted_x << std::endl;
    //std::cout << weighted_y << std::endl;
    x_centre = std::round(weighted_x/sum_of_masses);
    y_centre = std::round(weighted_y/sum_of_masses);
    //std::cout << x_centre << std::endl;
    //std::cout << y_centre << std::endl;
    x_centres.push_back(x_centre);
    y_centres.push_back(y_centre); //or store in one array?

}


void Shape_Feature::get_image_centres(const int size)
{
    x_centres.clear();
    y_centres.clear();
    int count = 0;
    std::cout << colour_images.size() << std::endl;
    for(int i = 0; i < colour_images.size(); i++)
    {
        get_weighted_centre(i, size);
    }
     std::cout << "No. of image centres " << (x_centres.size())  << std::endl;
}



void Shape_Feature::get_distances_from_centre(const int index, const int size)
{
    int * dist = new int[size];
    //int * y_dist = new int[size];
    for(int i = 0; i < size; i++)
    {
        dist[i] = 0;
        //y_dist[i] = 0;
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
            dist[i] = std::sqrt(std::pow(x_dist, 2) + std::pow(y_dist, 2));
        }
    }
    distances.push_back(dist); //all distances for either an R, G or B element
    //y_distances.push_back(y_dist);

}


void Shape_Feature::get_distances(const int size)
{
    distances.clear();
    //y_distances.clear();
    for(int i = 0; i < colour_images.size(); i++)
    {
        get_distances_from_centre(i, size);
    }
     std::cout << "No. of image centres " << (x_centres.size())  << std::endl;
}


void Shape_Feature::get_colour_images(const int size)
{
    colour_images.clear();
    int count = 0;
    std::cout << images.size() << std::endl;
    for(int i = 0; i < images.size(); i++)
    {
        split_into_RGB(i, size);
    }
     std::cout << "No. of colour images " << (colour_images.size())  << std::endl;
}



void Shape_Feature::split_into_RGB(const int index, const int size)
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
    ///*get_weighted_centre(int index, int size)
    colour_images.push_back(R_values);
    colour_images.push_back(G_values);
    colour_images.push_back(B_values);
   
}




//NB this assumes size of image is constant
int Shape_Feature::get_image_features(const int bin_size, const int size)
{
    std::cout << "No. of colour images " << (colour_images.size())/3  << std::endl;
    get_distances(size);
    int largest_dist = 21; //make more formal
    int hist_size = std::ceil((largest_dist+1)/(float)bin_size); //change back if necessary
    for(int i = 0; i < colour_images.size(); i+=3)
    {
        create_histogram(i, hist_size, bin_size, size); 
        create_histogram(i+1, hist_size, bin_size, size); 
        create_histogram(i+2, hist_size, bin_size, size); 
        //need a way to concatenate these?
    }
    //concatenate R, G and B histograms into one feature for each image
    //int hist_size = 255/bin_size;
    combine_histograms(hist_size);
    return hist_size;
}


void Shape_Feature::create_histogram(const int index, const int hist_size, const int bin, const int size)
{
    
    //use grey_images 
    //int hist_size = std::ceil((MAX_VAL+1)/bin);
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
     /*for (int j = 0; j < (maxVal+1); j++)
    {
            std::cout << frequencies[j] << " ";
    }*/
     group_in_bins(frequencies, hist_size, bin, (largest_dist+1));

}


void Shape_Feature::group_in_bins(const int frequencies[], const int hist_size, const int bin_size, const int size)
{
    //based on given bin size, group histogram array 
    //std::cout << "Grouping..." << std::endl;
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
            if(i > (size-1)) //check for values exceeding max value
            {
                break;
            }
            hist[count] += frequencies[i];
        }

    }
    image_features.push_back(hist);
    /*std::cout << "NEW IMAGE" << std::endl;
    for (int j = 0; j < hist_size; j++)
    {
            std::cout << hist[j] << " ";
    }*/

}




void Shape_Feature::combine_histograms(const int hist_size)
{
    for(int i = 0; i < image_features.size(); i+=3)
    {   
       concat_arrays(i, hist_size);
    }
    std::cout << combined_features.size() << std::endl;
     for(int i = 0; i < combined_features.size(); i+=3)
    {   
        //need a way to concatenate these?
        std::cout << "NEW IMAGE" << i <<  std::endl;
        for (int j = 0; j < hist_size*3; j++)
        {
            std::cout << combined_features[i][j] << " ";
        }
    }
}


void Shape_Feature::concat_arrays(const int index, const int hist_size)
{
    int * result = new int[hist_size*3];
    std::copy(image_features[index], image_features[index] + hist_size, result);
    std::copy(image_features[index+1], image_features[index+1] + hist_size, result + hist_size);  
    std::copy(image_features[index+2], image_features[index+2] + hist_size, result + (hist_size*2));  
    combined_features.push_back(result);

}


/*// deleting the 2d array
if (array[k][l] == 2.0 ) 
for(int i=0; i<rows; ++i)
{ delete [] array[i]; } // Delete the inner arrays
delete [] array;*/


 std::vector<int *>  Shape_Feature::get_image_features()
 {
     //return image_features;
     return combined_features;
 }


std::vector<std::string>  Shape_Feature::get_image_names()
{
    return image_names;
}