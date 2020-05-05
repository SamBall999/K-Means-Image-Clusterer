#ifndef SHAPEHIST_H
#define SHAPEHIST_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>



namespace BLLSAM009 {

    class Shape_Feature{

        private:
        int MAX_VAL;
        int OPTIMAL_BIN_WIDTH = 2;
        int width;
        int height;
        int size;
        int hist_size;
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  colour_images;
        std::vector<int>  x_centres;
        std::vector<int>  y_centres;
        std::vector<int *>  distances;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;
        std::vector<int *>  combined_features;


        public:
        //Constructors and Destructors
        Shape_Feature(void); 
        ~Shape_Feature(void); 


        //File I/O
        void read_images(const std::string & folder_name);
        std::string get_file_names(const std::string & folder_name) const;
        void read_image(const std::string & image_name);
        

        //Get shape feature
        void calculate_image_centres();
        void calc_weighted_centre(const int index); 
        void calc_distances();
        void calc_distances_from_centre(const int index);
        
        //Generate histogram features
        void process_colour_images();
        void split_into_RGB(const int index);
        void calculate_image_features(const int bin_size); 
        void create_histogram(const int index, const int bin_size);
        void group_in_bins(const int frequencies[], const int bin_size);
        void concat_arrays(const int index);
        void combine_histograms();
        std::vector<int *> get_image_features();
        std::vector<std::string> get_image_names();
        int get_hist_size();

    
    };


}

#endif