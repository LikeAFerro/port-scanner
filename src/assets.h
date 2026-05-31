#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>

#define ADDRESS_SIZE 16

typedef enum { OPEN, CLOSED, FILTERED, UNKNOWN } result_t;

result_t scan_port(const char* ip, uint16_t port);

#endif /* ASSETS_H */