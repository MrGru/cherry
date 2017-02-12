#include <cherry/graphic/node/node_tree.h>
#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>

/*
 * detachs
 */
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

static inline void detach_color(struct node_tree *p)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                branch_color_free(ob);
        }
}


static inline void detach_texcoord(struct node_tree *p)
{
        if( ! list_singular(&p->texcoord)) {
                struct list_head *head = p->texcoord.next;
                struct twig_texcoord *ob = (struct twig_texcoord *)
                        ((void *)head - offsetof(struct twig_texcoord, tree_head));
                twig_texcoord_free(ob);
        }
}


static inline void detach_texid(struct node_tree *p)
{
        if( ! list_singular(&p->texid)) {
                struct list_head *head = p->texid.next;
                struct twig_texid *ob = (struct twig_texid *)
                        ((void *)head - offsetof(struct twig_texid, tree_head));
                twig_texid_free(ob);
        }
}

/*
 * node allocations
 */
struct node_tree *node_tree_alloc(struct node *n)
{
        struct node_tree *p = smalloc(sizeof(struct node_tree));
        INIT_LIST_HEAD(&p->node_head);
        INIT_LIST_HEAD(&p->z);
        INIT_LIST_HEAD(&p->transform);
        INIT_LIST_HEAD(&p->color);
        INIT_LIST_HEAD(&p->texid);
        INIT_LIST_HEAD(&p->texcoord);
        INIT_LIST_HEAD(&p->life_head);
        node_tree_set_node(p, n);
        return p;
}

void node_tree_free(struct node_tree *p)
{
        detach_node(p);
        detach_z(p);
        detach_transform(p);
        detach_color(p);
        detach_texcoord(p);
        detach_texid(p);
        list_del(&p->life_head);
        sfree(p);
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

void node_tree_add_node_tree(struct node_tree *p, struct node_tree *q)
{
        branch_z_add(node_tree_get_branch_z(p),
                node_tree_get_branch_z(q));
        branch_transform_add(node_tree_get_branch_transform(p),
                node_tree_get_branch_transform(q));
        branch_color_add(node_tree_get_branch_color(p),
                node_tree_get_branch_color(q));
}

/*
 * setup branchs
 */
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

void node_tree_set_branch_color(struct node_tree *p, struct branch_color *b)
{
        detach_color(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->color);
        b->offset_to_node = offsetof(struct node_tree, color);
}

void node_tree_set_twig_texcoord(struct node_tree *p, struct twig_texcoord *b)
{
        detach_texcoord(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->texcoord);
        b->offset_to_node = offsetof(struct node_tree, texcoord);
        twig_texcoord_update(b);
}

void node_tree_set_twig_texid(struct node_tree *p, struct twig_texid *b)
{
        detach_texid(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->texid);
        b->offset_to_node = offsetof(struct node_tree, texid);
        twig_texid_update(b);
}

struct branch_z *node_tree_get_branch_z(struct node_tree *p)
{
        if( ! list_singular(&p->z)) {
                struct list_head *head = p->z.next;
                struct branch_z *ob = (struct branch_z *)
                        ((void *)head - offsetof(struct branch_z, tree_head));
                return ob;
        }
        return NULL;
}

struct branch_transform *node_tree_get_branch_transform(struct node_tree *p)
{
        if( ! list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return ob;
        }
        return NULL;
}

struct branch_color *node_tree_get_branch_color(struct node_tree *p)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                return ob;
        }
        return NULL;
}

/*
 * manipulate datas
 */
void node_tree_set_position(struct node_tree *p, union vec3 v)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                ob->update = 1;
                ob->position = v;
        }
}

void node_tree_set_size(struct node_tree *p, union vec3 size)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                ob->update = 1;
                ob->size = size;
        }
}

void node_tree_set_scale(struct node_tree *p, union vec3 v)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                ob->update = 1;
                ob->scale = v;
        }
}

void node_tree_set_rotation(struct node_tree *p, union vec4 quat)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                ob->update = 1;
                ob->quat = quat;
        }
}

// void node_tree_set_texcoord(struct node_tree *p, union vec2 root, union vec2 range)
// {
//         if( ! list_singular(&p->texroot)) {
//                 struct list_head *head = p->texroot.next;
//                 struct twig_texroot *ob = (struct twig_texroot *)
//                         ((void *)head - offsetof(struct twig_texroot, tree_head));
//                 ob->root = root;
//                 twig_texroot_update(ob);
//         }
//         if( ! list_singular(&p->texrange)) {
//                 struct list_head *head = p->texrange.next;
//                 struct twig_texrange *ob = (struct twig_texrange *)
//                         ((void *)head - offsetof(struct twig_texrange, tree_head));
//                 ob->range = range;
//                 twig_texrange_update(ob);
//         }
// }

void node_tree_set_texcoord(struct node_tree *p, u8 id, union vec2 coord, u8 update)
{
        if( ! list_singular(&p->texcoord)) {
                struct list_head *head = p->texcoord.next;
                struct twig_texcoord *ob = (struct twig_texcoord *)
                        ((void *)head - offsetof(struct twig_texcoord, tree_head));
                ob->texcoord[id] = coord;
                if(update) twig_texcoord_update(ob);
        }
}

void node_tree_set_color(struct node_tree *p, union vec4 color)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                ob->color = color;
        }
}

void node_tree_set_texid(struct node_tree *p, u8 id)
{
        if( ! list_singular(&p->texid)) {
                struct list_head *head = p->texid.next;
                struct twig_texid *ob = (struct twig_texid *)
                        ((void *)head - offsetof(struct twig_texid, tree_head));
                ob->texid = id + 0.001f;
                twig_texid_update(ob);
        }
}

union vec3 *node_tree_get_position(struct node_tree *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->position;
        }
        return NULL;
}

union vec3 *node_tree_get_scale(struct node_tree *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->scale;
        }
        return NULL;
}

union vec4 *node_tree_get_rotation(struct node_tree *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->quat;
        }
        return NULL;
}
