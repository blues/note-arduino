#ifndef HELPER_H
#define HELPER_H

#include <stddef.h>

void send_binary_data_to_notehub(void);
int send_binary_data_to_notecard(const void *data, size_t len);
void setup_binary_buffer(size_t buf_size);

#endif // HELPER_H
