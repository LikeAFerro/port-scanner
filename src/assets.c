#include "assets.h"
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

parse_result_t parse_arguments(int argc, char *argv[], config_t *config) {
    // Check if the required arguments are provided (at least the IP address)
    if (argc < 2) {
        return INVALID_ARGUMENTS;
    }

    int opt;
    static struct option long_options[] = {OPTIONS(MAKE_OPTION){NULL, 0, NULL, 0}};

    bool single_port_provided = false, min_port_provided = false, max_port_provided = false;

    // Use getopt_long to parse command-line options and their arguments
    while ((opt = getopt_long(argc, argv, OPTSTRING, long_options, NULL)) != -1) {
        switch (opt) {
        case 'p':
            if (string_to_port(optarg, &config->min_port) != OK) {
                return INVALID_PORT;
            }
            single_port_provided = true;
            break;
        case 'f':
            if (string_to_port(optarg, &config->min_port) != OK) {
                return INVALID_PORT;
            }
            min_port_provided = true;
            break;
        case 't':
            if (string_to_port(optarg, &config->max_port) != OK) {
                return INVALID_PORT;
            }
            max_port_provided = true;
            break;
        case 'v':
            config->verbose = true;
            break;
        case 'h':
            return HELP;
        case ':':
            return INVALID_ARGUMENTS;
            break;
        case '?':
            return INVALID_ARGUMENTS;
        }
    }

    // The remaining non-option argument should be the target IP address
    if (optind >= argc) {
        return INVALID_ARGUMENTS;
    }
    snprintf(config->ip, sizeof(config->ip), "%s", argv[optind]);

    // Validate that the port options are consistent (e.g., single port cannot be used with from/to)
    if (single_port_provided && (min_port_provided || max_port_provided)) {
        return INVALID_ARGUMENTS;
    }
    if (single_port_provided) {
        config->max_port = config->min_port;
    } else {
        if (!min_port_provided) {
            config->min_port = 1;
        }
        if (!max_port_provided) {
            config->max_port = 65535;
        }
        if (config->min_port > config->max_port) {
            return INVALID_ARGUMENTS;
        }
    }
    return OK;
}

parse_result_t string_to_port(const char *str, uint16_t *port) {
    char *endptr;
    errno = 0;
    unsigned long val = strtoul(str, &endptr, 10);
    if (*endptr != '\0' || val == 0 || val > 65535 || errno == ERANGE) {
        return INVALID_PORT;
    }
    *port = (uint16_t)val;
    return OK;
}

scan_result_t scan_port(const config_t *config, uint16_t port) {
    int fd;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Allow for both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    // Use getaddrinfo to resolve the IP address and port into a list of address structures
    if (getaddrinfo(config->ip, port_str, &hints, &res) != 0) {
        return UNKNOWN;
    }

    scan_result_t final_result = UNKNOWN;

    // Iterate through the list of address structures returned by getaddrinfo and attempt to connect
    // to each one
    for (p = res; p; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) {
            continue; // Try the next address if socket creation fails
        }

        // Set a timeout for the connect operation to avoid hanging indefinitely
        struct timeval timeout = {1, 0};
        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
            close(fd);
            continue;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
            close(fd);
            continue;
        }

        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            close(fd);
            final_result = OPEN;
            break; // Stop scanning if we find an open port
        } else {
            int err = errno;
            close(fd);
            if (err == ECONNREFUSED) {
                final_result = CLOSED;
            } else if (err == ETIMEDOUT || err == EAGAIN || err == EINPROGRESS ||
                       err == ENETUNREACH || err == EHOSTUNREACH) {
                final_result = FILTERED;
            } else {
                final_result = UNKNOWN;
            }
        }
        // Continue trying other addresses if the connection fails, as some addresses may be
        // filtered while others are open or closed
    }

    freeaddrinfo(res); // Free the address information allocated by getaddrinfo

    return final_result;
}

void help(void) {
    printf("Usage: port-scanner {target_ip} [options]\n");
    printf("Example: port-scanner 192.168.1.1 -f 1 -t 1000\n");
    printf("Target IP address is required. Options:\n");
    OPTIONS(MAKE_HELP)
}
