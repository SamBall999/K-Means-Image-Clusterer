
#include <iostream> 
#include <fstream>
#include <sstream> 
#include <vector>
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
                      std::cout << output_name << std::endl;
                      break;
                }

                case 'k':  
                {
                     std::cout << argv[i+1] << std::endl;
                    no_clusters = stoi(std::string(argv[i+1]));
                    break;
                }

                case 'b': 
                {
                     std::cout << argv[i+1] << std::endl;
                    bin_width = stof(std::string(argv[i+1])); //must this be bin not b??
                    break;
                }
            }
        }
       

    }

    std::cout << data_folder << std::endl;
    std::cout << output_name << std::endl;
    std::cout << no_clusters << std::endl;
    std::cout << bin_width << std::endl;



    //BLLSAM009::Clusterer * c = new BLLSAM009::Clusterer();
    BLLSAM009::Clusterer c;
    int size = c.read_images(data_folder);
    c.convert_to_grey(size);
     int hist_size = 255/bin_width;  //how to get max val ??
    c.get_image_features(bin_width, size);
    c.k_means(no_clusters, hist_size);
     std::cout << c << std::endl;


    //clean up memory in the destructor??
    //delete c; //clean up
    
    
    return 0;

}