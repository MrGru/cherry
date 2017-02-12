#ifndef __CHERRY_GRAPHIC_NODE_NODE_TREE_H__
#define __CHERRY_GRAPHIC_NODE_NODE_TREE_H__

#include <cherry/graphic/node/types.h>

/*
 * node_tree allocations
 */
struct node_tree *node_tree_alloc(struct node *n);

void node_tree_free(struct node_tree *p);

void node_tree_set_node(struct node_tree *p, struct node *n);

void node_tree_add_node_tree(struct node_tree *p, struct node_tree *q);

/*
 * node_tree setup branchs and twigs
 */
void node_tree_set_branch_z(struct node_tree *p, struct branch_z *b);

void node_tree_set_branch_transform(struct node_tree *p, struct branch_transform *b);

void node_tree_set_branch_color(struct node_tree *p, struct branch_color *b);

void node_tree_set_twig_texroot(struct node_tree *p, struct twig_texroot *b);

void node_tree_set_twig_texrange(struct node_tree *p, struct twig_texrange *b);

void node_tree_set_twig_texid(struct node_tree *p, struct twig_texid *b);

struct branch_z *node_tree_get_branch_z(struct node_tree *p);

struct branch_transform *node_tree_get_branch_transform(struct node_tree *p);

struct branch_color *node_tree_get_branch_color(struct node_tree *p);

/*
 * node_tree manipulate datas
 */
void node_tree_set_position(struct node_tree *p, union vec3 v);

void node_tree_set_scale(struct node_tree *p, union vec3 v);

void node_tree_set_rotation(struct node_tree *p, union vec4 quat);

void node_tree_set_size(struct node_tree *p, union vec3 size);

void node_tree_set_texcoord(struct node_tree *p, union vec2 root, union vec2 range);

void node_tree_set_texid(struct node_tree *p, u8 id);

void node_tree_set_color(struct node_tree *p, union vec4 color);

union vec3 *node_tree_get_position(struct node_tree *p);

union vec3 *node_tree_get_scale(struct node_tree *p);

union vec4 *node_tree_get_rotation(struct node_tree *p);

#endif
