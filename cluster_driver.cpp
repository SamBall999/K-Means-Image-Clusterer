
#include <iostream> 
#include <fstream>
#include <sstream> 
#include <vector>
#include <cmath>
#include <string>
#include "clusterer.h"


//command line, as follows: clusterer <dataset> [-o output] [-k n] [-bin b]

// dataset is the location of the folder that contains the images to be clustered.


//three optional command line arguments:
/*1. [-o output]: This is the name of the file that your program will write to. If no output
file is supplied, the program should default to writing the final clusterings to std:cout
2. [-k n]: This is the number of clusters to be generated by the K-means algorithm. It
should default to 10.
3. [-bin b]: This parameter specifies the width of your histogram feature. It should
default to 1.*/

//parse command line parameters
int main(int argc, char* argv[])
{
    //get file names
    std::string data_folder = std::string(argv[1]); 
    
    std::string output_name = "";
    int no_clusters = 10; //set default 
    float bin_width = 1; //set default
    bool colour_flag = false;

    //check for additional arguments
    if(argc > 1)
    {
        for(int i = 2; i < argc; i+=2)
        {
            //NB check flags 
            switch(std::string(argv[i]).at(1))
            {
                case 'o': 
                {
                      output_name = std::string(argv[i+1]);
                      //std::cout << output_name << std::endl;
                      break;
                }

                case 'k':  
                {
                     //std::cout << argv[i+1] << std::endl;
                    no_clusters = stoi(std::string(argv[i+1]));
                    break;
                }

                case 'b': 
                {
                    //std::cout << argv[i+1] << std::endl;
                    if(std::string(argv[i]) == "-bin") //check that valid option entered
                    {
                        bin_width = stof(std::string(argv[i+1])); //must this be bin not b??
                    }
                    break;
                }
                 case 'c': 
                {
                    //std::cout << argv[i] << std::endl;
                    if(std::string(argv[i]) == "-color") //check that valid option entered
                    {
                        colour_flag = true; //must this be colour not c?
                        //std::cout << "Use colour histograms" << std::endl;
                    }
                    break;
                }
                
            }
        }
       

    }

    std::cout << "\nCHOSEN PARAMETERS" << std::endl;
    std::cout << "Data Folder: " << data_folder << std::endl;
    std::cout << "Output File: " << output_name << std::endl;
    std::cout << "No. of Clusters: " << no_clusters << std::endl;
    std::cout << "Bin Width: " << bin_width << std::endl;
    std::cout << "Colour Histograms: " << colour_flag << std::endl;
    std::cout << "\n" << std::endl;



    std::string output_file_name = output_name + ".txt";
    //delete output file if it exists
      if (std::remove(output_file_name.c_str()) != 0)
     {
		//std::cout << "File does not exist" << std::endl;
     }


    if(colour_flag==true)
    {
         //read in images and get image features
        BLLSAM009::Colour_Feature col;
        int size = col.read_images(data_folder);
        //int hist_size = std::ceil(255/bin_width);  //how to get max val - keep as instance variable?? - 255 or 256?
        int hist_size = 255/bin_width; 
        std::cout << "Hist Size" << hist_size << std::endl;
        //col.split_into_RGB(size);
        col.get_colour_images(size);
        col.get_image_features(bin_width, size);

        //k means
        BLLSAM009::Clusterer c(col); //pass in greyscale feature
        c.k_means(no_clusters, hist_size*3); //hist_size??

         //write to output file
        std::ofstream out_file(output_file_name);
        out_file << c << std::endl;

    }
    else 
    {
        //read in images and get image features
        BLLSAM009::Grey_Feature g;
        int size = g.read_images(data_folder);
        //int hist_size = 255/bin_width;  //need to use ceil?? how to get max val - keep as instance variable??
        int hist_size = 255/bin_width; 
        g.convert_to_grey(size);
        g.get_image_features(bin_width, size);

        //k means
        BLLSAM009::Clusterer c(g); //pass in greyscale feature
        c.k_means(no_clusters, hist_size);

         //write to output file
        std::ofstream out_file(output_file_name);
        out_file << c << std::endl;
    }


    //BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer();
    /*BLLSAM009::Clusterer c;

    int size = c.read_images(data_folder);
    int hist_size = 255/bin_width;  //how to get max val ??
    //c.split_into_RGB(size);
    //c.get_colour_image_features(bin_width, size);


    c.convert_to_grey(size);
    c.get_image_features(bin_width, size);
    c.k_means(no_clusters, hist_size);
    // std::cout << c << std::endl;*/



    
    /*std::string output_file_name = output_name + ".txt";
    
    //delete output file if it exists
      if (std::remove(output_file_name.c_str()) != 0)
     {
		std::cout << "File does not exist" << std::endl;
     }

     //write to output file
      std::ofstream out_file(output_file_name);
     //out_file.open(output_file_name);
     //not printing to file??
     out_file << c << std::endl;
      //std::cout << c << std::endl;


    //clean up memory in the destructor??
    //delete c; //clean up*/
    
    
    return 0;

}