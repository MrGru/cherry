#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>

struct branch_color *branch_color_alloc(u8 bid)
{
        struct branch_color *p  = smalloc(sizeof(struct branch_color));
        p->bid                  = bid;
        p->pass                 = 0;
        p->color                = vec4((float[4]){1, 1, 1, 1});
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->branch_list);
        INIT_LIST_HEAD(&p->branch_head);
        return p;
}

void branch_color_free(struct branch_color *p)
{
        list_del(&p->tree_head);
        list_del(&p->branch_head);
        struct list_head *head;
        list_while_not_singular(head, &p->branch_list) {
                struct branch_color *b = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, branch_head));
                branch_color_free(b);
        }
        sfree(p);
}

void branch_color_add(struct branch_color *parent, struct branch_color *child)
{
        if(!parent || !child) return;
        
        list_del(&child->branch_head);
        list_add_tail(&child->branch_head, &parent->branch_list);
}

void branch_color_del(struct branch_color *p)
{
        list_del(&p->branch_head);
}

void branch_color_traverse(struct branch_color *p, union vec4 cc)
{
        p->display_color = vec4_mul(p->color, cc);

        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, p->display_color.v, sizeof(p->display_color));
                }
        }

        struct list_head *head;
        list_for_each(head, &p->branch_list) {
                struct branch_color *b = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, branch_head));
                branch_color_traverse(b, p->pass ? p->display_color
                                                 : vec4((float[4]){1, 1, 1, 1}));
        }
}
