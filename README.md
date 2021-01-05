# K-Means-Image-Clusterer


## Program Structure
The K Means Clustering program is comprised of twelve files including 
- clusterer.h
- clusterer.cpp
- grey_feature.h
- grey_feature.cpp 
- colour_feature.h
- colour_feature.cpp
- two_d_feature.h
- two_d_feature.cpp
- shape_feature.h
- shape_feature.cpp
- cluster_driver.cpp
- and a Makefile.
The program therefore consists of five header files, five source files, a driver file and a make file to compile and run the program.
Lastly, a sample folder of images Gradient_Numbers_PPMS has been included in order for the default 'make run' command to function correctly.

## File Descriptions
The cluster_driver.cpp file contains code needed to parse the command line arguments as well as to create an instance of the desired feature class and the clusterer class. The cluster_driver calls the necessary methods to read in the images, process the images, obtain the features and run the K Means algorithm.
Lastly, the driver class writes the clusters to the output file as well as to the terminal.

The clusterer.cpp source file contains the methods needed to initialise the centroids, assign the data points to the nearest centroids, recalculate the cluster means and check for convergence.
It should be noted that two methods of initialisation have been implemented, random initialisation of cluster means as well as kmeans++ initialisation which aims to ensure initial cluster means are as divergent as possible.
Kmeans++ has been left as the default initialisation as it achieves improved results.
The clusterer.cpp file also contains the appropriate constructors and destructor necessary to initialise the K Means Clusterer for each type of feature.

The clusterer.h file contains the class structure of the Clusterer class together with the namespace, function declarations and instance variables to hold the image features and cluster means. 
The clusterer.h file also contains the declaration of the overloaded stream operator.  

The grey_feature.cpp, colour_feature.cpp, two_d_feature.cpp and shape_feature.cpp source files all contain the necessary methods to read in the images, convert them into the respective histograms and group them into bins based on the given bin size.
The details of each histogram feature will be discussed below. The corresponding header files contain the class structure for each feature class as well as the instance variables to hold the images and image features. 

The Makefile is used to compile the source files into .o files.
The .o files are then linked into an executable called clusterer. This process is achieved by calling 'make'.
The command 'make run' can then be used in order to run the program. This will run the program with default command line parameters: an output file named "output", 10 clusters, a bin size of 1 and using greyscale features.
Thus it should be noted that the output file name is expected without a file extension.
This is the most basic form of the program as it uses the most simple features. Improved results are achieved through the addition of flags to use more complex features as discussed below.


## Command Line Argument Format
The command line format is as follows:
        clusterer <dataset> [-o output_name] [-k no_clusters] [-bin bin_width] [-<feature_flag>]   
where <feature_flag> can be:
        -color
        -two_dim
        -shape
depending on the type of feature to be used by the clusterer.
If any of the optional fields are left out, the default behaviour for that field will be used.
The default behaviour is defined as 10 clusters, a bin width of 1, greyscale features and the output being written to std::cout.
It should be noted that only one feature flag can be used at a time. 

## Feature Histograms
### 1. Greyscale
The grey_feature.cpp source file contains methods to convert the images to greyscale and then create a histogram of pixel intensities from 0 to 255.

### 2. Colour 
The colour_feature.cpp source file contains methods to group the pixels of each image into R, G and B arrays and then create a histogram of pixel intensities from 0 to 255 for each of the channels.
It also contains methods to combine the three histograms into one feature of format [R_channel_hist, G_channel_hist, B_channel_hist]. 

ADDITIONAL FEATURES
Two additional features were implemented to improve performance of the clustering algorithm. 

### 3. Two Dimensional
In order to improve performance, a two-dimensional colour histogram was implemented. 
The basis of this more complex feature is that not only does it describe the distribution of the pixel intensities in each of the R, G and B channels but it also describes the correlation between pairs of channels e.g. between the R and G channels.
The 2D histogram is computed by grouping the pixel intensities into R, G and B channels and then creating a 2D array counting the frequency of each pixel for each pair of values e.g. for each (R, G) pair.
Three 2D histograms were computed, between R and G, G and B and B and R respectively. The three histograms were then combined into one feature of format [R_G_2D_hist, G_B_2Dhist, B_R_2Dhist].
Lastly, the combined 2D histogram was flattened into a 1D array for use in the cluster algorithm.
Using the two dimensional feature, the algorithm was observed to cluster almost perfectly.
Through experimentation the optimal bin size for this feature was found to be 10.

 The idea for creating a two dimensional histogram was inspired by the following resource:
 https://www.pyimagesearch.com/2014/01/22/clever-girl-a-guide-to-utilizing-color-histograms-for-computer-vision-and-image-search-engines/

### 4. Shape 
Lastly, a feature histogram based on the shape of the image was computed.
In order to encode spatial information, the centre of mass of each image was calculated in the form (x_centre, y_centre).
This was performed for each of the R, G and B channels respectively. 
The distances from each non-zero (colour) pixel to the centre of mass were then calculated and converted into a histogram feature.
Lastly, the three histograms were combined into one feature of format [distances_to_centre_R, distances_to_centre_G, distances_to_centre_B].
Using the shape feature, the algorithm was observed to cluster perfectly for the optimal bin size of 2.

 The idea for creating a shape encoding histogram using centre of mass was inspired by the following resource:
 https://www.cse.usf.edu/~r1k/MachineVisionBook/MachineVision.files/MachineVision_Chapter2.pdf
