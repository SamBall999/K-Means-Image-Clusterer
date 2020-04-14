#ifndef COLOURFEATURE_H
#define COLOURFEATURE_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>



namespace BLLSAM009 {

    class Colour_Feature{

        private:
        int MAX_VAL;
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  colour_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;
        std::vector<int *>  combined_features;


        public:
        //Constructors and Destructors
        Colour_Feature(void); // default constructor 
        ~Colour_Feature(void); //destructor


        //File I/O
        int read_images(const std::string & folder_name);
        int read_image(const std::string & image_name);
        void write_to_output();
        

        //Generate histogram features
        void get_colour_images(int size);
        void split_into_RGB(int index, int size);
        int get_image_features(int bin_size, int size);
        void create_histogram(int index, int hist_size, int bin, int size);
        void group_in_bins(const int frequencies[], int hist_size, int bin_size, int size);
        void combine_histograms(int hist_size);
        void concat_arrays(int index, int hist_size);
        std::vector<int *>  get_image_features();
        std::vector<std::string>  get_image_names();

    
    };





}

#endif