/*
 * device buffer definitions
 */
#ifndef __CHERRY_GRAPHIC_DEVICE_BUFFER_H__
#define __CHERRY_GRAPHIC_DEVICE_BUFFER_H__

#include <cherry/graphic/types.h>

/*
 * allocate new device buffer
 */
struct device_buffer *device_buffer_alloc();

/*
 * resize buffer and fill size bytes
 * @p           : buffer to resize/file
 * @bytes       : bytes array
 * @size        : bytes array length
 */
void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size);

/*
 * @p           : buffer to change
 * @offset      : index that buffer changes from
 * @bytes       : bytes array
 * @size        : bytes array length
 */
void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size);

/*
 * deallocate device buffer
 */
void device_buffer_free(struct device_buffer *p);

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc();

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p);

#endif
