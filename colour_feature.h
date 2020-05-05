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
        int size;
        int hist_size;
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  colour_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;
        std::vector<int *>  combined_features;


        public:
        //Constructors and Destructors
        Colour_Feature(void);
        ~Colour_Feature(void);


        //File I/O
        void read_images(const std::string & folder_name);
        std::string get_file_names(const std::string & folder_name) const;
        void read_image(const std::string & image_name);
        

        //Generate histogram features
        void process_colour_images();
        void split_into_RGB(const int index);
        void calculate_image_features(const int bin_size);
        void create_histogram(const int index, const int bin_size);
        void group_in_bins(const int frequencies[], const int bin_size);
        int get_hist_size(); 

        //Combine features from R, G and B channels
        void combine_histograms();
        void concat_arrays(const int index);
        std::vector<int *> get_image_features();
        std::vector<std::string> get_image_names();

    
    };





}

#endif