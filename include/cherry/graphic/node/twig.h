#ifndef __CHERRY_GRAPHIC_NODE_TWIG_H__
#define __CHERRY_GRAPHIC_NODE_TWIG_H__

#include <cherry/graphic/node/types.h>

/*
 * twig texcoord definitions
 */
struct twig_texcoord *twig_texcoord_alloc(u8 bid[6]);

void twig_texcoord_free(struct twig_texcoord *p);

void twig_texcoord_update(struct twig_texcoord *p);

/*
 * twig texid definitions
 */
struct twig_texid *twig_texid_alloc(u8 bid);

void twig_texid_free(struct twig_texid *p);

void twig_texid_update(struct twig_texid *p);

#endif
