CC=g++ #the compiler
LIBS=  #the libraries 

#Link into executable
clusterer: cluster_driver.o clusterer.o 
	$(CC) -o clusterer cluster_driver.o clusterer.o $(LIBS)

#Compile
cluster_driver.o: cluster_driver.cpp 
	$(CC) -c cluster_driver.cpp  --std=c++11

#Compile
clusterer.o: clusterer.cpp clusterer.h
	$(CC) -c clusterer.cpp  --std=c++11

#Other rules including clean and run 
clean:
	@rm -f *.o clusterer 

run:
	./clusterer Gradient_Numbers_PPMS -o output -k 10 -bin 10