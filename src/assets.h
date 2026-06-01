#ifndef ASSETS_H
#define ASSETS_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

#define OPTIONS(X)                                                                                 \
    X("port", required_argument, 'p', "  -p, --port {port}   Scan a single port")                  \
    X("from", required_argument, 'f', "  -f, --from {port}   Minimum port")                        \
    X("to", required_argument, 't', "  -t, --to {port}     Maximum port")                          \
    X("verbose", no_argument, 'v', "  -v, --verbose       Show closed ports")                      \
    X("help", no_argument, 'h', "  -h, --help          Show this message")

#define OPTSTRING ":p:f:t:hv"
#define MAKE_HELP(name, has_arg, val, desc) printf("%s\n", desc);
#define MAKE_OPTION(name, has_arg, val, desc) {name, has_arg, NULL, val},

typedef enum { OPEN, CLOSED, FILTERED, UNKNOWN } scan_result_t;

typedef enum { OK, INVALID_ARGUMENTS, INVALID_PORT, HELP } parse_result_t;
typedef struct {
    char ip[INET6_ADDRSTRLEN];
    uint16_t min_port;
    uint16_t max_port;
    bool verbose;
} config_t;

parse_result_t parse_arguments(int argc, char *argv[], config_t *config);
parse_result_t string_to_port(const char *str, uint16_t *port);
scan_result_t scan_port(const config_t *config, uint16_t port);
void help(void);

#endif /* ASSETS_H */
