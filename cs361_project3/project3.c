/*
 * project3.c
 *
 * Adrien Ponce & Nic Plybon
 * This code adheres to the JMU Honor Code
 */

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

        // Check if the current cluster is invalid
        // and set the file attribute to 0x80
        if (classification == TYPE_UNCLASSIFIED) {
            classification = TYPE_UNKNOWN;
        }

        // Writes the current clusters file attribute
        // to the classification file and move to the
        // next cluster
        write(classification_fd, &classification, 1);
        cluster_number++;
    }

    close(input_fd);

    // Try opening the file for reading, exit with appropriate error message
    // if open fails
    if (classification_fd < 0) {
        printf("Error opening file \"%s\": %s\n", CLASSIFICATION_FILE, strerror(errno));
        return 1;
    }

    // Creates and opens a map file for writing to
    map_fd = open(MAP_FILE, O_RDWR | O_CREAT, 0600);

    // Reset classification offset in order to read from the beginning
    lseek(classification_fd, 0, SEEK_SET);

    // Keeps track of individual type offsets for map entry writing
    uint32_t jpg_offset = 0x31303030;
    uint32_t html_offset = 0x31303030;

    // Keeps track of offset in the file for map entry writing
    uint32_t cluster_count = 0;

    // Iterate over each cluster, reading in the cluster type attributes
    while ((bytes_read = read(classification_fd, &cluster_type, 1)) > 0) {

        // For every cluster, filename starts with the 'file' constant
        write(map_fd, "file", 4);

        switch (cluster_type) {
        // Cluster is type JPG Header
        case 3:
            write(map_fd, &jpg_offset, 4);
            break;
        // Cluster is type JPG Body
        case 1:
            write(map_fd, &jpg_offset, 4);
            break;
        // Cluster is type JPG Footer
        case 5:
            jpg_offset += 0x01000000;

            // Once file number is at 9, increment the bit to the left
            // and continue incrementing the offset
            if ((jpg_offset & 0xff000000) == 0x39000000) {
                jpg_offset += 0x00010000;
                jpg_offset -= 0x09000000;
            }

            write(map_fd, &jpg_offset, 4);
            break;
        // Cluster is type HTML Header
        case 24:
            write(map_fd, &html_offset, 4);
            break;
        // Cluster is type HTML Body
        case 8:
            write(map_fd, &html_offset, 4);
            break;
        // Cluster is type HTML Footer
        case 40:
            html_offset += 0x01000000;

            // Once file number is at 9, increment the bit to the left
            // and continue incrementing the offset
            if ((html_offset & 0xff000000) == 0x39000000) {
                html_offset += 0x00010000;
                html_offset -= 0x09000000;
            }

            write(map_fd, &html_offset, 4);
            break;
        // Cluster is a single JPG
        case 7:
            jpg_offset += 0x01000000;

            // Once file number is at 9, increment the bit to the left
            // and continue incrementing the offset
            if ((jpg_offset & 0xff000000) == 0x39000000) {
                jpg_offset += 0x00010000;
                jpg_offset -= 0x09000000;
            }

            write(map_fd, &jpg_offset, 4);
            break;
        // Cluster is a single HTML
        case 56:
            html_offset += 0x01000000;

            // Once file number is at 9, increment the bit to the left
            // and continue incrementing the offset
            if ((html_offset & 0xff000000) == 0x39000000) {
                html_offset += 0x00010000;
                html_offset -= 0x09000000;
            }

            write(map_fd, &html_offset, 4);
            break;
        }

        switch(cluster_type) {
        // If is of type JPG write .jpg to the file
        case 3:
        case 1:
        case 5:
        case 7:
            write(map_fd, ".jpg", 4);
            break;
        // If is of type HTML write .html to the file
        case 24:
        case 8:
        case 40:
        case 56:
            write(map_fd, ".htm", 4);
            break;
        }

        // Write non-printable ASCII cluster number to the end of the cluster
        // and move to the next cluster in the classification file
        write(map_fd, &cluster_count, 4);
        cluster_count++;
    }

    // Resource cleanup
    close(classification_fd);
    close(map_fd);

    return 0;
}
