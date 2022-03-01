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
    int map_fd;
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
    
    // Try opening the file for reading, exit with appropriate error message
    // if open fails
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0) {
        printf("Error opening file \"%s\" for reading: %s\n", argv[1], strerror(errno));
        return 1;
    }

    // Creates and opens a classifcation file for writing to
    classification_fd = open(CLASSIFICATION_FILE, O_RDWR | O_CREAT, 0600); 

    // Used to keep track of clusters in the while loops
    cluster_number = 0;

    // Iterate through all the clusters, reading their contents 
    // into cluster_data
    while ((bytes_read = read(input_fd, &cluster_data, CLUSTER_SIZE)) > 0) {
        assert(bytes_read == CLUSTER_SIZE);
        classification = TYPE_UNCLASSIFIED;

        /*
            In this loop, you need to implement the functionality of the
            classifier. Each cluster needs to be examined using the functions
            provided in classify.c. Then for each cluster, the attributes
            need to be written to the classification file.
        */
   
        // Checks that the current cluster is of type JPG by looking for
        // its body, and if it has either a header or a footer on each 
        // iteration. This helps avoid false negatives
        if (has_jpg_body(cluster_data)) {
            classification = TYPE_IS_JPG;
            if (has_jpg_header(cluster_data) 
                | has_jpg_footer(cluster_data)) {
                classification = TYPE_IS_JPG;
            }
        }
        
        // Checks that the current cluster is of type HTML by looking for
        // its body, and if it has either a header or a footer on each 
        // iteration. This helps avoid false negatives
        if (has_html_body(cluster_data)) {
            classification = TYPE_IS_HTML;
            if (has_html_header(cluster_data) 
                | has_html_footer(cluster_data)) {
                classification = TYPE_IS_HTML;
            }  
        } 
        
        // Check if the current cluster is a JPG header
        // and set the file attribute to 0x03
        if (has_jpg_header(cluster_data)) {
            classification = TYPE_IS_JPG | TYPE_JPG_HEADER;
        } 

        // Check if the current cluster is a HTML header
        // and set the file attribute to 0x18
        if (has_html_header(cluster_data)) {
            classification = TYPE_IS_HTML | TYPE_HTML_HEADER;
        } 

        // Check if the current cluster is a JPG footer
        // and set the file attribute to 0x05
        if (has_jpg_footer(cluster_data)) {
            classification = TYPE_IS_JPG | TYPE_JPG_FOOTER;
        }

        // Check if the current cluster is a HTML footer
        // and set the file attribute to 0x28
        if (has_html_footer(cluster_data)) {
            classification = TYPE_IS_HTML | TYPE_HTML_FOOTER;
        }

        // Single HTML contains both a header and a footer followed by zero bytes.
        // Set the file attribtue to 0x38
        if (has_html_header(cluster_data) 
            && has_html_footer(cluster_data)) {
            classification = TYPE_IS_HTML | TYPE_HTML_HEADER | TYPE_HTML_FOOTER;
        } 
        // Single JPG contains both a header and a footer followed by zero bytes.
        // Set the file attribute to 0x07
        if (has_jpg_header(cluster_data) 
            && has_jpg_footer(cluster_data)) {
            classification = TYPE_IS_JPG | TYPE_JPG_HEADER | TYPE_JPG_FOOTER;
        } 

        // Writes the current clusters file attribute
        // to the classification file and move to the
        // next cluster 
        write(classification_fd, &classification, 1);
        cluster_number++;
    }
    
    close(input_fd);

    // Creates and opens a map file for writing to
    map_fd = open(MAP_FILE, O_RDWR | O_CREAT, 0600); 

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


        //printf("%d\n", cluster_type);

       
    }
    
    close(classification_fd);
}
