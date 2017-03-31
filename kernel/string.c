/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/array.h>

static inline void __string_reserve(struct string *p, size_t size)
{
        p->ptr = srealloc(p->ptr, size + 1);
}

static inline void __string_cat(struct string *p, char *src, size_t src_len)
{
        __string_reserve(p, p->len + src_len);
        smemcpy(p->ptr + p->len, src, src_len);
        p->len += src_len;
        p->ptr[p->len] = '\0';
}

static inline struct string *__string_alloc(size_t len)
{
        struct string *s = smalloc(sizeof(struct string));
        s->len = 0;
        s->ptr = smalloc(len + 1);
        s->ptr[0] = '\0';
        return s;
};

static inline void __string_free(struct string *p)
{
        sfree(p->ptr);
        sfree(p);
}

struct string *string_alloc(size_t len)
{
        return __string_alloc(len);
};

struct string *string_alloc_chars(char *s, size_t len)
{
        struct string *p = __string_alloc(len);
        __string_cat(p, s, len);
        return p;
}

void string_free(struct string *p)
{
        __string_free(p);
}

void string_reserve(struct string *p, size_t size)
{
        __string_reserve(p, size);
}

void string_cat(struct string *p, char *src, size_t src_len)
{
        __string_cat(p, src, src_len);
}

void string_cat_int(struct string *p, int number)
{
        int_to_chars(v, number)
        __string_cat(p, v, strlen(v));
}

static void __string_replace_inplace(struct string *s, char *search, u32 src_len,
        char *replace, u32 rpl_len)
{
	i32 start = -2;
	i32 end = -1;
	i32 count = 0;
	i32 first_between = -1;
	u32 i;
	for(i = 0; i < s->len; i++)
	{
		if(s->ptr[i] == search[count])
		{
			if(first_between == -1 && count > 0 && s->ptr[i] == search[0])
			{
				first_between = i;
			}
			end = i;
			if(count == 0) start = i;
			count++;
			if(count == src_len)
			{
				//replace
				smemcpy(s->ptr + start, replace, src_len);
				first_between = -1;
				start = i - 1;
				end = i;
				count = 0;
			}
		}
		else
		{
			count = 0;
			if(first_between > 0)
			{
				// jump to previous index matching first character
				i = first_between;
				first_between = -1;
				count++;
				start = i;
				end = i;
			}
			else
			{
				// move to next character
				start = i-1;
				end = i;
				if(s->ptr[i] == search[count])
				{
					// current index matches first character
					start = end;
					count++;
				}
			}
		}
	}

	if(end - start + 1 == src_len) // case src_len == 1
	{
		//replace
		smemcpy(s->ptr + start, replace, src_len);
	}
}

static u32 __string_find_occurence(struct string *s, char *search, u32 src_len, struct array *rem)
{
	u32 occur = 0;
	i32 start = -2;
	i32 end = -1;
	i32 count = 0;
	i32 first_between = -1;
	u32 i;
	for(i = 0; i < s->len; i++)
	{
		if(s->ptr[i] == search[count])
		{
			if(first_between == -1 && count > 0 && s->ptr[i] == search[0])
			{
				first_between = i;
			}
			end = i;
			if(count == 0) start = i;
			count++;
			if(count == src_len)
			{
				occur++;
                                array_push(rem, &start);
				array_push(rem, &end);
				first_between = -1;
				start = i - 1;
				end = i;
				count = 0;
			}
		}
		else
		{
			count = 0;
			if(first_between > 0)
			{
				// jump to previous index matching first character
				i = first_between;
				first_between = -1;
				count++;
				start = i;
				end = i;
			}
			else
			{
				// move to next character
				start = i-1;
				end = i;
				if(s->ptr[i] == search[count])
				{
					// current index matches first character
					start = end;
					count++;
				}
			}
		}
	}

	if(end - start + 1 == src_len) // case src_len == 1
	{
		array_push(rem, &start);
		array_push(rem, &end);
		occur++;
	}

	return occur;
}

void string_find(struct string *p, char *search, struct array *result)
{
        __string_find_occurence(p, search, strlen(search), result);
}

void string_replace_direct(struct string *p, u32 index, u32 range, char *replace, u32 replace_len)
{
        struct string *buff = __string_alloc(0);
        __string_cat(buff, p->ptr, index);
        __string_cat(buff, replace, replace_len);
        __string_cat(buff, p->ptr + index + range, p->len - (index +range));
        p->len = 0;
        __string_cat(p, buff->ptr, buff->len);
        __string_free(buff);
}

static void __string_replace_double_buffer(struct string *s, char* search,
        u32 src_len, char *replace, u32 rpl_len)
{
	u32 i;
	/* create rem to remember blocks [start, end] in s */
	struct array *rem = array_alloc(sizeof(u32), ORDERED);
	u32 occur = __string_find_occurence(s, search, src_len, rem);
	/* calculate return size */
	u32 ret_size = s->len - occur * src_len + occur * rpl_len;
	struct string *buff = __string_alloc(0);
	__string_reserve(buff, ret_size);
	/* replace */
	u32 index = 0;
	for(i = 0; i < rem->len; i+= 2)
	{
		u32 start = array_get(rem, u32, i);
		u32 end = array_get(rem, u32, i + 1);
		if(start > index)
		{
			__string_cat(buff, s->ptr+index, start - index);
		}
		__string_cat(buff, replace, rpl_len);
		index = end + 1;
	}
	/* fill tail */
	if(index < s->len)
	{
		__string_cat(buff, s->ptr+index, s->len - index);
	}
	/* free up rem */
	array_free(rem);
	/* replace s content by buff */
	s->len = 0;
	__string_cat(s, buff->ptr, buff->len);
	__string_free(buff);
}

void string_replace(struct string *p, char *search, char *replace)
{
        size_t src_size = strlen(search);
	size_t rpl_size = strlen(replace);

	if(src_size == rpl_size)
	{
		//in place
		__string_replace_inplace(p, search, src_size, replace, rpl_size);
	}
	else
	{
		//double buffer
		__string_replace_double_buffer(p, search, src_size, replace, rpl_size);
	}
}

u8 string_contain(struct string *s, char *search)
{
        size_t src_len = strlen(search);
	i32 start = -2;
	i32 end = -1;
	i32 count = 0;
	i32 first_between = -1;
	u32 i;
	for(i = 0; i < s->len; i++)
	{
		if(s->ptr[i] == search[count])
		{
			if(first_between == -1 && count > 0 && s->ptr[i] == search[0])
			{
				first_between = i;
			}
			end = i;
			if(count == 0) start = i;
			count++;
			if(count == src_len)
			{
				goto success;
			}
		}
		else
		{
			count = 0;
			if(first_between > 0)
			{
				// jump to previous index matching first character
				i = first_between;
				first_between = -1;
				count++;
				start = i;
				end = i;
			}
			else
			{
				// move to next character
				start = i-1;
				end = i;
				if(s->ptr[i] == search[count])
				{
					// current index matches first character
					start = end;
					count++;
				}
			}
		}
	}

	if(end - start + 1 == src_len) // case src_len == 1
	{
		goto success;
	}

fail:
	return 0;
success:
	return 1;
}
