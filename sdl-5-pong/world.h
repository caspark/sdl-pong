#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>

#include "entities.h"
#include "util.h"

class World {
public:
	Player *human;
	Player *opponent;
	Ball *ball;

	World(SDL_Renderer *renderer, int width, int height);
	~World();
	void startRound();
	void update();
	void render();

private:
	int width;
	int height;
};

#endif
