#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "grey_feature.h"
#include "colour_feature.h"


//break into several classes?

namespace BLLSAM009 {

    class Clusterer{

        private:
        //try to decrease these?
        //std::vector<std::unique_ptr<unsigned char *>> images;
        //std::vector<unsigned char *> images;
        //std::vector<unsigned char *>  greyscale_images;
        //std::vector<unsigned char *>  colour_images;
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;
        std::vector<int *>  cluster_means;
        std::map<int, std::vector<int>> cluster_map;
        bool has_converged;


        public:
        Clusterer(void); // default constructor 
        Clusterer(Grey_Feature grey);
        Clusterer(Colour_Feature grey);
        ~Clusterer(void); //destructor

        //File I/O
        /*int read_images(const std::string & folder_name);
        int read_image(const std::string & image_name);
        void write_to_output();
        void write_grey_to_output();
     
        //Convert to grey_scale
        void convert_to_grey(int size);
        void to_greyscale(int index, int size);
        unsigned char colour_conversion(unsigned char R, unsigned char G, unsigned char B);

        //Process colour images
        void split_into_RGB(int size);
        void get_colour_image_features(int bin_size, int size);

        //Get image features
        void create_histogram(int index, int maxVal, int bin, int size, bool colour);
        void get_image_features(int bin_size, int size);
        void group_in_bins(const int frequencies[], int hist_size, int bin_size, int size);*/

        //K means algorithm 
        float get_euclid_distance(int image_index, int cluster_index, int hist_size);
        void get_random_means(int no_clusters);
        void assign_to_cluster(int hist_size);
        bool update_bin_avgs(int cluster_index, int hist_size);
        void update_means(int hist_size);
        void k_means(int no_clusters, int hist_size);

        //Stream operator
        friend std::ostream & operator<<(std::ostream & os, const Clusterer& kt);
        
        //std::vector<unsigned int> extract_keys(std::map<unsigned int, int> const& map);
        //std::vector<int> extract_values(std::map<unsigned int, int> const& map);


    };


    std::ostream & operator<<(std::ostream & os, const Clusterer& kt);





}

#endif