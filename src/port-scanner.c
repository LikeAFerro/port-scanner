#include "assets.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // Check if the required arguments are provided (at least the IP address)
    if (argc < 2) {
        fprintf(stderr,
                "%s: Missing arguments. Check %s --help for usage information.\n",
                argv[0],
                argv[0]);
        exit(1);
    }

    // Check if the first argument is a valid IP address (not starting with '-')
    if (argv[1][0] == '-') {
        if (argv[1][1] == 'h' || strcmp(argv[1], "--help") == 0) {
            help();
            exit(0);
        }
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        exit(1);
    }
    char ip[INET6_ADDRSTRLEN];
    snprintf(ip, sizeof(ip), "%s", argv[1]);

    int opt;
    struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
        {"from", required_argument, NULL, 'f'},
        {"to", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'h'},
        {"verbose", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0} // Sentinel to mark the end of the array
    };
    uint16_t port, min_port, max_port;
    bool port_provided = false, min_port_provided = false, max_port_provided = false,
         verbose = false;

    // Use getopt to parse command-line options and their arguments
    while ((opt = getopt_long(argc, argv, ":p:f:t:hv", long_options, NULL)) != -1) {
        switch (opt) {
        case 'p':
            if (string_to_port(optarg, &port) == -1) {
                fprintf(stderr, "Invalid port number: %s\n", optarg);
                exit(1);
            }
            port_provided = true;
            break;
        case 'f':
            if (string_to_port(optarg, &min_port) == -1) {
                fprintf(stderr, "Invalid minimum port number: %s\n", optarg);
                exit(1);
            }
            min_port_provided = true;
            break;
        case 't':
            if (string_to_port(optarg, &max_port) == -1) {
                fprintf(stderr, "Invalid maximum port number: %s\n", optarg);
                exit(1);
            }
            max_port_provided = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            help();
            exit(0);
        case ':':
            fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            exit(1);
            break;
        case '?':
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            exit(1);
        }
    }

    // Scan the specified ports based on the provided options
    if (port_provided && (min_port_provided || max_port_provided)) {
        fprintf(stderr, "Cannot specify both -p and -f/-t options.\n");
        exit(1);
    }
    if (port_provided) {
        min_port = max_port = port;
    } else {
        if (!min_port_provided) {
            min_port = 1;
        }
        if (!max_port_provided) {
            max_port = 65535;
        }
        if (min_port > max_port) {
            fprintf(stderr, "Minimum port cannot be greater than maximum port.\n");
            exit(1);
        }
    }

    for (int p = min_port; p <= max_port; p++) {
        // Call the scan_port function to check the status of the specified port on
        // the given IP address
        result_t result = scan_port(ip, p);

        // Print the result of the port scan based on the value returned by
        // scan_port
        switch (result) {
        case OPEN:
            printf("Port %d is OPEN on %s\n", p, ip);
            break;
        case CLOSED:
            if (verbose) {
                printf("Port %d is CLOSED on %s\n", p, ip);
            }
            break;
        case FILTERED:
            printf("Port %d is FILTERED on %s\n", p, ip);
            break;
        default:
            printf("Port %d is UNKNOWN on %s\n", p, ip);
            break;
        }
    }

    return 0;
}
