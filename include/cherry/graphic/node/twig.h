#ifndef __CHERRY_GRAPHIC_NODE_TWIG_H__
#define __CHERRY_GRAPHIC_NODE_TWIG_H__

#include <cherry/graphic/node/types.h>

/*
 * twig root definitions
 */
struct twig_texroot *twig_texroot_alloc(u8 bid);

void twig_texroot_free(struct twig_texroot *p);

void twig_texroot_update(struct twig_texroot *p);

/*
 * twig range definitions
 */
struct twig_texrange *twig_texrange_alloc(u8 bid);

void twig_texrange_free(struct twig_texrange *p);

void twig_texrange_update(struct twig_texrange *p);

/*
 * twig texid definitions
 */
struct twig_texid *twig_texid_alloc(u8 bid);

void twig_texid_free(struct twig_texid *p);

void twig_texid_update(struct twig_texid *p);

#endif
