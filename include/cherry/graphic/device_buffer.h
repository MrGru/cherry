/*
 * device buffer definitions
 */
#ifndef __CHERRY_GRAPHIC_DEVICE_BUFFER_H__
#define __CHERRY_GRAPHIC_DEVICE_BUFFER_H__

#include <cherry/graphic/types.h>

struct device_buffer_head *device_buffer_alloc();

void device_buffer_fill(struct device_buffer_head *head, void *bytes, uint64_t size);

void device_buffer_sub(struct device_buffer_head *head, uint64_t offset, void *bytes, uint64_t size);

void device_buffer_free(struct device_buffer_head *head);

#endif
