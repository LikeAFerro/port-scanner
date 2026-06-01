#include "assets.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

result_t scan_port(const char *ip, uint16_t port) {
    // Create a file descriptor for a TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        // If socket creation fails, print an error message and exit with status 1
        return UNKNOWN;
    }

    // Define a structure to hold the server address
    struct sockaddr_in server_addr;
    // Set the address family to IPv4
    server_addr.sin_family = AF_INET;
    // Set the port number to the provided port and convert it to network byte
    // order
    server_addr.sin_port = htons(port);
    // Convert the IP address from text to binary form and store it in the
    // structure
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", ip);
        // If the IP address is invalid, print an error message and exit with status
        // 1
        close(fd);
        return UNKNOWN;
    }

    // Define a structure to specify the timeout for sending data on the socket
    struct timeval timeout = {1, 0};
    // Set the socket option to specify the send and receive timeout for the
    // socket
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt(SO_SNDTIMEO)");
        close(fd);
        return UNKNOWN;
    }
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt(SO_RCVTIMEO)");
        close(fd);
        return UNKNOWN;
    }

    result_t result;
    // Attempt to connect to the server using the specified address and port
    // Note that the casting is necessary because connect() expects a pointer to a
    // struct sockaddr, and server_addr is of type struct sockaddr_in
    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        result = OPEN;
    } else {
        int err = errno;
        if (err == ECONNREFUSED) {
            result = CLOSED;
        } else if (err == ETIMEDOUT || err == EAGAIN || err == EINPROGRESS || err == ENETUNREACH ||
                   err == EHOSTUNREACH) {
            result = FILTERED;
        } else {
            result = UNKNOWN;
        }
    }

    // Close the socket file descriptor
    close(fd);

    return result;
}

int string_to_port(const char *str, uint16_t *port) {
    char *endptr;
    errno = 0;
    unsigned long val = strtoul(str, &endptr, 10);
    if (*endptr != '\0' || val == 0 || val > 65535 || errno == ERANGE) {
        return -1;
    }
    *port = (uint16_t)val;
    return 0;
}

void help(void) {
    printf("Usage: port-scanner {target_ip} [options]\n");
    printf("Example: port-scanner 192.168.1.1 -f 1 -t 1000\n");
    printf("Target IP address is required. Options:\n");
    printf("  -p {port}   Specify a single port to scan\n");
    printf("  -f {port}   Specify the minimum port to scan\n");
    printf("  -t {port}   Specify the maximum port to scan\n");
    printf("  -v          Enable verbose output (show closed ports)\n");
    printf("  -h, --help  Display this help message\n");
    printf("If no options are provided, the program will scan all ports from 1 to 65535.\n");
    printf("Return values:\n");
    printf("0: Scan completed successfully\n");
    printf("1: Invalid command-line arguments\n");
}
