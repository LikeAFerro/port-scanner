#ifndef ASSETS_H
#define ASSETS_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Command-line options definition (X-Macro).
 *
 * Used to generate getopt_long options and help output consistently.
 */
#define OPTIONS(X)                                                                                 \
    X("port", required_argument, 'p', "  -p, --port {port}   Scan a single port")                  \
    X("from", required_argument, 'f', "  -f, --from {port}   Minimum port")                        \
    X("to", required_argument, 't', "  -t, --to {port}     Maximum port")                          \
    X("verbose", no_argument, 'v', "  -v, --verbose       Show closed ports")                      \
    X("help", no_argument, 'h', "  -h, --help          Show this message")

/** @brief The option string for getopt_long */
#define OPTSTRING ":p:f:t:hv"
/** @brief Macro to generate the help message for each option */
#define MAKE_HELP(name, has_arg, val, desc) printf("%s\n", desc);
/** @brief Macro to generate the option definitions for getopt_long */
#define MAKE_OPTION(name, has_arg, val, desc) {name, has_arg, NULL, val},

/** @brief The type for the result of a port scan */
typedef enum { OPEN, CLOSED, FILTERED, UNKNOWN } scan_result_t;

/** @brief The type for the result of parsing command-line arguments */
typedef enum { OK, INVALID_ARGUMENTS, INVALID_PORT, HELP } parse_result_t;

/** @brief The configuration for the port scanner, based on the command-line arguments */
typedef struct {
    char ip[INET6_ADDRSTRLEN];
    uint16_t min_port;
    uint16_t max_port;
    bool verbose;
} config_t;

/**
 * @brief Parse the command-line arguments and populate the configuration structure
 * @param argc The number of command-line arguments
 * @param argv The array of command-line argument strings
 * @param config A pointer to the configuration structure to populate based on the parsed arguments
 * @return A parse_result_t indicating the result of parsing the arguments (OK, INVALID_ARGUMENTS,
 * INVALID_PORT, or HELP)
 */
parse_result_t parse_arguments(int argc, char *argv[], config_t *config);

/**
 * @brief Convert a string to a port number
 * @param str The string to convert
 * @param port A pointer to the port number to populate
 * @return A parse_result_t indicating the result of the conversion (OK or INVALID_PORT)
 */
parse_result_t string_to_port(const char *str, uint16_t *port);

/**
 * @brief  * This function attempts a TCP connection to determine port state.
 * It does not retry and uses blocking connect().
 * @param config A pointer to the configuration structure
 * @param port The port number to scan
 * @return A scan_result_t indicating the result of the scan (OPEN, CLOSED, FILTERED, or UNKNOWN)
 */
scan_result_t scan_port(const config_t *config, uint16_t port);

/** @brief Display the help message for the port scanner */
void help(void);

#endif /* ASSETS_H */
