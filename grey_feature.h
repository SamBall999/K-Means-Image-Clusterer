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
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  greyscale_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;


        public:
        //Constructors and Destructors
        Grey_Feature(void); // default constructor 
        ~Grey_Feature(void); //destructor


        //File I/O
        int read_images(const std::string & folder_name);
        int read_image(const std::string & image_name);
        void write_to_output();
        
        //Convert to grey_scale
        void convert_to_grey(int size);
        void to_greyscale(int index, int size);
        unsigned char colour_conversion(unsigned char R, unsigned char G, unsigned char B);
        void write_grey_to_output();

        //Generate histogram features
        void get_image_features(int bin_size, int size);
        void create_histogram(int index, int maxVal, int bin, int size);
        void group_in_bins(const int frequencies[], int hist_size, int bin_size, int size);
        std::vector<int *>  get_image_features();
        std::vector<std::string>  get_image_names();



    
    };





}

#endif