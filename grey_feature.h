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
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  greyscale_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;


        public:
        //Constructors and Destructors
        Grey_Feature(void); // default constructor 
        ~Grey_Feature(void); //destructor


        //File I/O
        int read_images(const std::string & folder_name); //how to get this to only be a writer? ??? why is this necessary?
        std::string get_file_names(const std::string & folder_name) const;
        int read_image(const std::string & image_name);
        void write_to_output() const;
        
        //Convert to grey_scale
        void convert_to_grey(const int size);
        void to_greyscale(const int index, const int size);
        unsigned char colour_conversion(const unsigned char R, const unsigned char G, const unsigned char B) const;
        void write_grey_to_output() const; 

        //Generate histogram features
        int get_image_features(const int bin_size, const int size);
        void create_histogram(const int index, const int hist_size, const int bin, const int size);
        void group_in_bins(const int frequencies[], const int hist_size, const int bin_size, const int size);
        std::vector<int *> get_image_features();
        std::vector<std::string> get_image_names();



    
    };





}

#endif