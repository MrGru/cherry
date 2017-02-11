#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/math/math.h>
#include <cherry/memory.h>

struct branch_transform *branch_transform_alloc(u8 bid)
{
        struct branch_transform *p      = smalloc(sizeof(struct branch_transform));
        p->bid                          = bid;
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->branch_list);
        INIT_LIST_HEAD(&p->branch_head);
        return p;
}

void branch_transform_free(struct branch_transform *p)
{
        list_del(&p->tree_head);
        list_del(&p->branch_head);
        struct list_head *head;
        list_while_not_singular(head, &p->branch_list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, branch_head));
                branch_transform_free(b);
        }
        sfree(p);
}

void branch_transform_add(struct branch_transform *parent, struct branch_transform *child)
{
        list_del(&child->branch_head);
        list_add_tail(&child->branch_head, &parent->branch_list);
}

void branch_transform_del(struct branch_transform *p)
{
        list_del(&p->branch_head);
}

void branch_transform_traverse(struct branch_transform *p, union mat4 cm)
{
        if(!p->update) return;
        p->update = 0;

        union mat4 transform = mat4_identity;
        transform = mat4_translate(transform, p->position);
        transform = mat4_mul(transform, mat4_from_quat(p->quat));
        transform = mat4_scale(transform, p->scale);
        transform = mat4_mul(cm, transform);

        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, transform.m, sizeof(transform));
                }
        }

        struct list_head *head;
        list_for_each(head, &p->branch_list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, branch_head));
                branch_transform_traverse(b, transform);
        }
}