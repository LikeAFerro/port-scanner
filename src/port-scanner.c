#include "assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {

    // Check if the user provided an IP address as a command-line argument
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        exit(1);
    }

    // Copy the provided IP address into a local variable for further use
    char ip[ADDRESS_SIZE];
    snprintf(ip, sizeof(ip), "%s", argv[1]);

    // Convert the provided port number from a string to an integer
    char* end;
    errno = 0;
    unsigned long tmp = strtoul(argv[2], &end, 10);
    // Check if the port number is valid (between 1 and 65535)
    if (*end != '\0' || tmp > 65535 || errno == ERANGE) {
        fprintf(stderr, "Invalid port number: %s\n", argv[2]);
        exit(1);
    }
    uint16_t port = (uint16_t)tmp;

    // Call the scan_port function to check the status of the specified port on the given IP address
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

    return 0;
}
