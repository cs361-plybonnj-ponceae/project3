/* project3.c */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#include "common.h"
#include "classify.h"

int main(int argc, char *argv[])
{
    int input_fd;
    int classification_fd;
    int cluster_number;
    int bytes_read;
    unsigned char classification, cluster_type;
    char cluster_data[CLUSTER_SIZE];
    
    // We only accept running with one command line argumet: the name of the
    // data file
    if (argc != 2) {
        printf("Usage: %s data_file\n", argv[0]);
        return 1;
    }

    // Creates (on first iteration) then opens the file in read/write mode with read/write 
    // owner permissions
    classification_fd = open(CLASSIFICATION_FILE, O_RDWR | O_CREAT, 0600); 
  
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0) {
        printf("Error opening file \"%s\" for reading: %s\n", argv[1], strerror(errno));
        return 1;
    }
    
    // Iterate through all the clusters, reading their contents 
    // into cluster_data
    cluster_number = 0;
    while ((bytes_read = read(input_fd, &cluster_data, CLUSTER_SIZE)) > 0) {
        assert(bytes_read == CLUSTER_SIZE);
        classification = TYPE_UNCLASSIFIED;

      
        /*
            In this loop, you need to implement the functionality of the
            classifier. Each cluster needs to be examined using the functions
            provided in classify.c. Then for each cluster, the attributes
            need to be written to the classification file.
        */
      
      if (has_jpg_body(&cluster_data[cluster_number])) {
        classification = TYPE_IS_JPG;
      }
      
      if ((has_jpg_header(&cluster_data[cluster_number])
        && has_jpg_body(&cluster_data[cluster_number]))) {
        classification = TYPE_IS_JPG + TYPE_JPG_HEADER;
      }

      if (has_jpg_body(&cluster_data[cluster_number])) {
        classification = TYPE_IS_JPG;
      }

      if (has_jpg_footer(&cluster_data[cluster_number])) {
        classification = TYPE_IS_JPG + TYPE_JPG_FOOTER;
      }

    

        write(classification_fd, &classification, 1);
        cluster_number++;
    }
        
      }
    
    close(input_fd);

    // Try opening the classification file for reading, exit with appropriate
    // error message if open fails 
    if (classification_fd < 0) {
        printf("Error opening file \"%s\": %s\n", CLASSIFICATION_FILE, strerror(errno));
        return 1;
    }
    
    // Iterate over each cluster, reading in the cluster type attributes
    while ((bytes_read = read(classification_fd, &cluster_type, 1)) > 0) {
        /*
            In this loop, you need to implement the functionality of the
            mapper. For each cluster, a map entry needs to be created and
            written to the map file.
        */
    }
    
    close(classification_fd);

    return 0;
};
