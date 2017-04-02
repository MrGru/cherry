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
#include <cherry/graphic/update_task.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/graphic/node_manager.h>

struct update_task *update_task_request(struct node_manager *manager, void *data, update_task_callback callback, int count)
{
        struct update_task *p   = smalloc(sizeof(struct update_task));
        p->count                = count;
        p->callback             = callback;
        p->data                 = data;
        list_add_tail(&p->head, &manager->update_tasks);
        return p;
}

void update_task_free(struct update_task *p)
{
        list_del(&p->head);
        sfree(p);
}
