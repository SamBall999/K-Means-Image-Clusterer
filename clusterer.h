#ifndef CLUSTERER_H
#define CLUSTERER_H


#include <vector>
#include <string>
#include <map>


//break into several classes?

namespace BLLSAM009 {

    class Clusterer{

        private:
        //try to decrease these?
        std::vector<unsigned char *> images;
        std::vector<unsigned char *>  greyscale_images;
        std::vector<int *>  image_features;
        std::vector<int *>  cluster_means;
        std::map<int, std::vector<int>> cluster_map;


        public:
        Clusterer(void); // default constructor 
        ~Clusterer(void); //destructor
        int read_images(const std::string & folder_name);
        //void to_greyscale(unsigned char * image, int size);
        void to_greyscale(int index, int size);
        unsigned char colour_conversion(unsigned char R, unsigned char G, unsigned char B);
        int * create_histogram(int index, int maxVal, int bin, int size);
        void get_image_features(int bin_size, int size);
        //std::vector<unsigned int> extract_keys(std::map<unsigned int, int> const& map);
        //std::vector<int> extract_values(std::map<unsigned int, int> const& map);
        void group_in_bins(const int frequencies[], int hist[], int bin_size, int size);
        float get_euclid_distance(int hist1[], int hist2[], int hist_size);
        void get_random_means(int num_clusters);
        void assign_to_cluster(int size, int bin_size, int num_clusters);
        void get_cluster_avg(int cluster_index, int hist_size);
        int* get_bin_avgs(int cluster_index, int hist_size);
        void update_means(int hist_size);
        void k_means();


    };





}

#endif