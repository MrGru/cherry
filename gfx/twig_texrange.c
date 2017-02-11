#include <cherry/graphic/node/twig.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>

struct twig_texrange *twig_texrange_alloc(u8 bid)
{
        struct twig_texrange *p = smalloc(sizeof(struct twig_texrange));
        p->range                = vec2((float[2]){1, 1});
        p->bid                  = bid;
        INIT_LIST_HEAD(&p->tree_head);
        return p;
}

void twig_texrange_free(struct twig_texrange *p)
{
        list_del(&p->tree_head);
        sfree(p);
}

void twig_texrange_update(struct twig_texrange *p)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, p->range.v, sizeof(p->range));
                }
        }
}
