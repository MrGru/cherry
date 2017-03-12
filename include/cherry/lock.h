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
#ifndef __CHERRY_LOCK_H__
#define __CHERRY_LOCK_H__

#include <cherry/types.h>
#include <errno.h>

typedef int spin_lock;

static inline int spin_lock_init(spin_lock *lock, int pshared) {
	__asm__ __volatile__ ("" ::: "memory");
	*lock = 0;
	return 0;
}

static inline int spin_lock_destroy(spin_lock *lock) {
	return 0;
}

static inline int spin_lock_lock(spin_lock *lock) {
	while (1) {
		int i;
		for (i=0; i < 10000; i++) {
			if (__sync_bool_compare_and_swap(lock, 0, 1)) {
				return 0;
			}
		}
		sched_yield();
	}
}

static inline int spin_lock_trylock(spin_lock *lock) {
	if (__sync_bool_compare_and_swap(lock, 0, 1)) {
		return 0;
	}
	return EBUSY;
}

static inline int spin_lock_unlock(spin_lock *lock) {
	__asm__ __volatile__ ("" ::: "memory");
	*lock = 0;
	return 0;
}

#endif
