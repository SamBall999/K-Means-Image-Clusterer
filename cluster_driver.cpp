
#include <iostream> 
#include <fstream>
#include <sstream> 
#include <vector>
#include <cmath>
#include <string>
#include <memory>
#include "clusterer.h"


//parse command line parameters
int main(int argc, char* argv[])
{
    //get folder name
    std::string data_folder = std::string(argv[1]); 
    
    //set default parameters if none given
    std::string output_name = ""; //intialise output file name
    bool write_to_file = false; //if no file name given, write to std::cout
    int no_clusters = 10; //set default 
    float bin_width = 1; //set default

    //flags to indicate use of more complex feature
    bool colour_flag = false;
    bool two_dim_flag = false;
    bool shape_flag = false;

    //check for additional arguments
    if(argc > 1)
    {
        for(int i = 2; i < argc; i+=2)
        {
            //check flags
            switch(std::string(argv[i]).at(1))
            {
                case 'o': 
                {
                      output_name = std::string(argv[i+1]); //set name of output file
                      write_to_file = true; //write to given file instead of std::cout
                      break;
                }

                case 'k':  
                {
                    no_clusters = stoi(std::string(argv[i+1])); //set number of clusters
                    break;
                }

                case 'b': 
                {
                    if(std::string(argv[i]) == "-bin") //check that valid option entered
                    {
                        bin_width = stof(std::string(argv[i+1])); //set bin width
                    }
                    break;
                }
                 case 'c': 
                {
                    if(std::string(argv[i]) == "-color") //check that valid option entered
                    {
                        colour_flag = true; //use colour histograms
                    }
                    break;
                }
                 case 't': 
                {
                    if(std::string(argv[i]) == "-two_dim") //check that valid option entered
                    {
                        two_dim_flag = true; //use two dimensional histograms
                    }
                    break;
                }
                  case 's': 
                {
                    if(std::string(argv[i]) == "-shape") //check that valid option entered
                    {
                        shape_flag = true; //use shape encoding histograms
                    }
                    break;
                }
                
            }
        }
       

    }

    //display chosen parameters
    std::cout << "\nCHOSEN PARAMETERS" << std::endl;
    std::cout << "Data Folder: " << data_folder << std::endl;
    if(write_to_file)
    {
        std::cout << "Output: " << output_name << std::endl;
    }
    else
    {
        std::cout << "Output: std::cout" << std::endl;
    }
    std::cout << "No. of Clusters: " << no_clusters << std::endl;
    std::cout << "Bin Width: " << bin_width << std::endl;
    std::cout << "Colour Histograms: " << colour_flag << std::endl;
    std::cout << "Two Dimensional Histograms: " << two_dim_flag << std::endl;
    std::cout << "Shape Histograms: " << shape_flag << std::endl;
    std::cout << "\n" << std::endl;


    //add file extension
    std::string output_file_name = output_name + ".txt";
    //delete output file if it exists
      if (std::remove(output_file_name.c_str()) != 0)
     {
		//file did not exist
     }


    //create correct feature according to command line arguments
    if(colour_flag==true)
    {
         //read in images and get image features
        BLLSAM009::Colour_Feature col;
        col.read_images(data_folder);
        col.calculate_image_features(bin_width);
        int hist_size = col.get_hist_size();

        //run k means on given features
        BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer(col); //pass in colour features
        c->k_means(no_clusters, hist_size); //run k means 

        //check if file name given
        if(write_to_file)
        {
            //write to output file
            std::ofstream out_file(output_file_name);
            out_file << *c << std::endl;
            std::cout << "Finished writing to file " << output_file_name << std::endl;
        }
        else
        {
            //write to std::cout if no output file givens
            std::cout << "\n" << std::endl;
            std::cout << *c << std::endl;
        }
        

    }
    else if (two_dim_flag == true)
    {
        //read in images and get image features
        BLLSAM009::Two_D_Feature t;
        t.read_images(data_folder);
        t.calculate_image_features(bin_width); 
        int hist_size = t.get_hist_size();

        //run k means on given features
        BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer(t); //pass in two dimensional features
        c->k_means(no_clusters, hist_size); //run k means

        //check if file name given
        if(write_to_file)
        {
            //write to output file
            std::ofstream out_file(output_file_name);
            out_file << *c << std::endl;
            std::cout << "Finished writing to file " << output_file_name << std::endl;
        }
        else
        {
            //write to std::cout if no output file givens
            std::cout << "\n" << std::endl;
            std::cout << *c << std::endl;
        }

    }
    else if (shape_flag == true)
    {
         {
        //read in images and get image features
        BLLSAM009::Shape_Feature s;
        s.read_images(data_folder);
        s.calculate_image_centres();
        s.calculate_image_features(bin_width);
        int hist_size = s.get_hist_size();

          
        //run k means on given features
        BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer(s); //pass in shape encoded features
        c->k_means(no_clusters, hist_size); //run k means

        //check if file name given
        if(write_to_file)
        {
            //write to output file
            std::ofstream out_file(output_file_name);
            out_file << *c << std::endl;
            std::cout << "Finished writing to file " << output_file_name << std::endl;
        }
        else
        {
            //write to std::cout if no output file givens
            std::cout << "\n" << std::endl;
            std::cout << *c << std::endl;
        }

    }
    }
    else 
    {
        //read in images and get image features
        BLLSAM009::Grey_Feature g;
        //BLLSAM009::Grey_Feature * g = new BLLSAM009::Grey_Feature();
        g.read_images(data_folder);
        g.convert_to_grey();
        g.get_image_features(bin_width);
        int hist_size = g.get_hist_size();

        //run k means on given features
        BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer(g); //pass in greyscale features
        c->k_means(no_clusters, hist_size); //run k means


        //check if file name given
        if(write_to_file)
        {
            //write to output file
            std::ofstream out_file(output_file_name);
            out_file << *c << std::endl;
            std::cout << "Finished writing to file " << output_file_name << std::endl;
        }
        else
        {
            //write to std::cout if no output file givens
            std::cout << "\n" << std::endl;
            std::cout << *c << std::endl;
        }

    }
    
    
    return 0;

}