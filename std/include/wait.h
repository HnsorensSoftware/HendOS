#ifndef WAIT_H
#define WAIT_H

#include <stdint.h>

uint64_t waitpid(uint64_t pid, uint64_t* status, uint64_t options);

#endif