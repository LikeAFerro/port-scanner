#include "assets.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    // Check if the user provided an IP address as a command-line argument
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <MIN_PORT> <MAX_PORT>\n", argv[0]);
        exit(1);
    }

    // Copy the provided IP address into a local variable for further use
    char ip[ADDRESS_SIZE];
    snprintf(ip, sizeof(ip), "%s", argv[1]);

    // Convert the provided port numbers from a string to an integer
    uint16_t min_port, max_port;
    if (string_to_port(argv[2], &min_port) == -1) {
        fprintf(stderr, "Invalid minimum port number: %s\n", argv[2]);
        exit(1);
    }
    if (string_to_port(argv[3], &max_port) == -1) {
        fprintf(stderr, "Invalid maximum port number: %s\n", argv[3]);
        exit(1);
    }

    // Check if the port numbers are valid (min_port should not be greater than max_port)
    if (min_port > max_port) {
        fprintf(stderr, "Minimum port number cannot be greater than maximum port number\n");
        exit(1);
    }

    for (int port = min_port; port <= max_port; port++) {
        // Call the scan_port function to check the status of the specified port on the given IP
        // address
        result_t result = scan_port(ip, port);

        // Print the result of the port scan based on the value returned by scan_port
        switch (result) {
            case OPEN:
                printf("Port %d is OPEN on %s\n", port, ip);
                break;
            case CLOSED:
                printf("Port %d is CLOSED on %s\n", port, ip);
                break;
            case FILTERED:
                printf("Port %d is FILTERED on %s\n", port, ip);
                break;
            default:
                printf("Port %d is UNKNOWN on %s\n", port, ip);
                break;
        }
    }

    return 0;
}
