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
        std::string get_file_names(const std::string & folder_name) const;
        int read_image(const std::string & image_name);
        void write_to_output() const;
        

        //Generate histogram features
        void get_colour_images(const int size);
        void split_into_RGB(const int index, const int size);
        int get_image_features(const int bin_size, const int size);
        void create_histogram(const int index, const int hist_size, const int bin, const int size);
        void group_in_bins(const int frequencies[], const int hist_size, const int bin_size, const int size);
        void combine_histograms(const int hist_size);
        void concat_arrays(const int index, const int hist_size);
        std::vector<int *> get_image_features();
        std::vector<std::string> get_image_names();

    
    };





}

#endif