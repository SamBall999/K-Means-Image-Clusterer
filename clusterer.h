#ifndef CLUSTERER_H
#define CLUSTERER_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "grey_feature.h"
#include "colour_feature.h"
#include "two_d_feature.h"
#include "three_d_feature.h"
#include "shape_feature.h"



namespace BLLSAM009 {

    class Clusterer{

        private:
        std::vector<std::string>  image_names;
        std::vector<int *>  image_features;
        std::vector<int *>  cluster_means;
        std::map<int, std::vector<int>> cluster_map;
        bool has_converged;

        //FOR TESTING ONLY
         std::vector<int> cluster_indexes;


        public:
        Clusterer(void); // default constructor 
        Clusterer(Grey_Feature grey);
        Clusterer(Colour_Feature colour);
        Clusterer(Two_D_Feature two_dim);
        Clusterer(Three_D_Feature three_dim);
        Clusterer(Shape_Feature shape);
        ~Clusterer(void); //destructor


        //K means algorithm 
        float get_euclid_distance(const int image_index, const int cluster_index, const int hist_size) const;
        float get_abs_distance(const int image_index, const int cluster_index, const int hist_size) const;
        void get_random_means(const int no_clusters);
        void k_meansplusplus_init(const int no_clusters, const int hist_size);
        void assign_to_cluster(const int hist_size);
        bool update_bin_avgs(const int cluster_index, const int hist_size); //returns status true or false for convergence
        void update_means(const int hist_size);
        void k_means(const int no_clusters, const int hist_size);

        //Stream operator
        friend std::ostream & operator<<(std::ostream & os, const Clusterer& kt);

        //Testing only
        void print_hists(int i, int k, int hist_size);
        
        //std::vector<unsigned int> extract_keys(std::map<unsigned int, int> const& map);
        //std::vector<int> extract_values(std::map<unsigned int, int> const& map);


    };


    std::ostream & operator<<(std::ostream & os, const Clusterer& kt);





}

#endif