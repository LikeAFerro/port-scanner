#include "assets.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    config_t config = {0};
    parse_result_t parse_result = parse_arguments(argc, argv, &config);
    switch (parse_result) {
    case INVALID_ARGUMENTS:
        fprintf(stderr, "Error parsing arguments. Check %s --help for usage information.\n",
                argv[0]);
        exit(1);
    case INVALID_PORT:
        fprintf(stderr, "Error: Invalid port number.\n");
        exit(1);
    case HELP:
        help();
        exit(0);
    case OK:
        break;
    }

    // Iterate through the specified port range and scan each port, printing the results to the
    // console. If the verbose flag is set, also print closed ports.
    for (int p = config.min_port; p <= config.max_port; p++) {
        scan_result_t result = scan_port(&config, p);

        switch (result) {
        case OPEN:
            printf("Port %d is OPEN on %s\n", p, config.ip);
            break;
        case CLOSED:
            if (config.verbose) {
                printf("Port %d is CLOSED on %s\n", p, config.ip);
            }
            break;
        case FILTERED:
            printf("Port %d is FILTERED on %s\n", p, config.ip);
            break;
        default:
            printf("Port %d is UNKNOWN on %s\n", p, config.ip);
            break;
        }
    }

    return 0;
}
