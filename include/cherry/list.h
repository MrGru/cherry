/*
 * linked list definition
 * inspired from linux kernel list head
 */
#ifndef __CHERRY_LIST_H__
#define __CHERRY_LIST_H__

#include <cherry/types.h>

/*
 * double linked list implementation
 */
#define LIST_HEAD_INIT(p) {&(p), &(p)}

static inline void INIT_LIST_HEAD(struct list_head *list)
{
        list->prev = list;
        list->next = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
        next->prev = new;
        new->next = next;
        new->prev = prev;
        prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
        __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
        __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
        next->prev = prev;
        prev->next = next;
}

static inline void list_del(struct list_head *head)
{
        __list_del(head->prev, head->next);
}

static inline int list_singular(struct list_head *head)
{
        return head == head->next;
}

#define list_for_each(p, list) \
        for(p = (list)->next; p != (list); p = p->next)

#define list_for_each_safe(p, k, list) \
        for(p = (list)->next, k = p->next; p != (list); p = k, k = p->next)

#endif
