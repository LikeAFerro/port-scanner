#ifndef ASSETS_H
#define ASSETS_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum { OPEN, CLOSED, FILTERED, UNKNOWN } scan_result_t;

typedef enum { OK, INVALID_ARGUMENTS, INVALID_IP_ADDRESS, INVALID_PORT, HELP } parse_result_t;
typedef struct {
    char ip[INET_ADDRSTRLEN];
    uint16_t min_port;
    uint16_t max_port;
    bool verbose;
} config_t;

parse_result_t parse_arguments(int argc, char *argv[], config_t *config);
parse_result_t string_to_port(const char *str, uint16_t *port);
scan_result_t scan_port(const char *ip, uint16_t port);
void help(void);

#endif /* ASSETS_H */
