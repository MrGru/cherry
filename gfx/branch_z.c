#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/memory.h>

struct branch_z *branch_z_alloc(u8 bid)
{
        struct branch_z *p      = smalloc(sizeof(struct branch_z));
        p->bid                  = bid;
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->branch_list);
        INIT_LIST_HEAD(&p->branch_head);
        return p;
}

void branch_z_free(struct branch_z *p)
{
        list_del(&p->tree_head);
        list_del(&p->branch_head);
        struct list_head *head;
        list_while_not_singular(head, &p->branch_list) {
                struct branch_z *b = (struct branch_z *)
                        ((void *)head - offsetof(struct branch_z, branch_head));
                branch_z_free(b);
        }
        sfree(p);
}

void branch_z_add(struct branch_z *parent, struct branch_z *child)
{
        if(!parent || !child) return;

        list_del(&child->branch_head);
        list_add_tail(&child->branch_head, &parent->branch_list);
}

void branch_z_del(struct branch_z *p)
{
        list_del(&p->branch_head);
}

void branch_z_traverse(struct branch_z *p, float *cz)
{
        *cz -= 0.00001f;
        p->z = *cz;
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, &p->z, sizeof(p->z));
                }
        }

        struct list_head *head;
        list_for_each(head, &p->branch_list) {
                struct branch_z *b = (struct branch_z *)
                        ((void *)head - offsetof(struct branch_z, branch_head));
                branch_z_traverse(b, cz);
        }
}
