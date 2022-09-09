#include "reverse_buffer.h"

void reverseBuffer(unsigned char *buffer, size_t size) {
    for (size_t i = 0; i < size / 2; ++i) {
        unsigned char tmp = buffer[size - i - 1];
        buffer[size - i - 1] = buffer[i];
        buffer[i] = tmp;
    }
}