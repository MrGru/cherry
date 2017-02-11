#ifndef __CHERRY_GRAPHIC_NODE_NODE_TREE_H__
#define __CHERRY_GRAPHIC_NODE_NODE_TREE_H__

#include <cherry/graphic/node/types.h>

struct node_tree *node_tree_alloc();

void node_tree_free(struct node_tree *p);

void node_tree_set_node(struct node_tree *p, struct node *n);

void node_tree_set_branch_z(struct node_tree *p, struct branch_z *b);

void node_tree_set_branch_transform(struct node_tree *p, struct branch_transform *b);

#endif
