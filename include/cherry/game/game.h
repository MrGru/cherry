#ifndef __CHERRY_GAME_GAME_H__
#define __CHERRY_GAME_GAME_H__

#include <cherry/game/types.h>

struct game *game_alloc();

void game_update(struct game *p);

void game_render(struct game *p);

void game_free(struct game *p);

#endif
