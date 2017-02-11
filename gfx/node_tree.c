#include <cherry/graphic/node/node_tree.h>
#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>

static inline void detach_node(struct node_tree *p)
{
        if(!list_singular(&p->node_head)) {
                struct list_head *head = p->node_head.next;
                struct node *node = (struct node *)
                        ((void *)head - offsetof(struct node, user_head));
                node_free(node);
        }
}

static inline void detach_z(struct node_tree *p)
{
        if( ! list_singular(&p->z)) {
                struct list_head *head = p->z.next;
                struct branch_z *ob = (struct branch_z *)
                        ((void *)head - offsetof(struct branch_z, tree_head));
                branch_z_free(ob);
        }
}

static inline void detach_transform(struct node_tree *p)
{
        if( ! list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                branch_transform_free(ob);
        }
}

struct node_tree *node_tree_alloc()
{
        struct node_tree *p = smalloc(sizeof(struct node_tree));
        INIT_LIST_HEAD(&p->node_head);
        INIT_LIST_HEAD(&p->z);
        INIT_LIST_HEAD(&p->transform);
        return p;
}

void node_tree_free(struct node_tree *p)
{
        detach_node(p);
        detach_z(p);
        detach_transform(p);
}

void node_tree_set_node(struct node_tree *p, struct node *n)
{
        /* detach previous node and push it back to pool */
        detach_node(p);
        /* detach current node_tree that node n is attaching to */
        list_del(&n->user_head);
        /* attach node n to node_tree p */
        list_add_tail(&n->user_head, &p->node_head);
}

void node_tree_set_branch_z(struct node_tree *p, struct branch_z *b)
{
        /* detach previous branch */
        detach_z(p);
        /* format branch b */
        list_del(&b->tree_head);
        /* attach branch b */
        list_add_tail(&b->tree_head, &p->z);
        b->offset_to_node = offsetof(struct node_tree, z);
}

void node_tree_set_branch_transform(struct node_tree *p, struct branch_transform *b)
{
        /* detach previous branch */
        detach_transform(p);
        /* format branch b */
        list_del(&b->tree_head);
        /* attach branch b */
        list_add_tail(&b->tree_head, &p->transform);
        b->offset_to_node = offsetof(struct node_tree, transform);
}
