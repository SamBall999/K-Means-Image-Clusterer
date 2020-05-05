#ifndef GREYFEATURE_H
#define GREYFEATURE_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>



namespace BLLSAM009 {

    class Grey_Feature{

        private:
        int MAX_VAL;
        int size;
        int hist_size;
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  greyscale_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;


        public:
        //Constructors and Destructors
        Grey_Feature(void); 
        ~Grey_Feature(void); 


        //File I/O
        void read_images(const std::string & folder_name); 
        std::string get_file_names(const std::string & folder_name) const;
        void read_image(const std::string & image_name);
        
        //Convert to grey_scale
        void convert_to_grey();
        void to_greyscale(const int index);
        unsigned char colour_conversion(const unsigned char R, const unsigned char G, const unsigned char B) const;

        //Generate histogram features
        void get_image_features(const int bin_size);
        void create_histogram(const int index, const int bin);
        void group_in_bins(const int frequencies[], const int bin_size);
        std::vector<int *> get_image_features();
        std::vector<std::string> get_image_names();
        int get_hist_size();



    
    };





}

#endif