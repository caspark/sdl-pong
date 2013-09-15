#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>

#include "entities.h"
#include "util.h"

#define PHYSICS_TIMESTEP 0.01f

class WorldState {
public:
	MovingRect human;
	int humanScore;
	MovingRect opponent;
	int opponentScore;
	MovingRect ball;

	WorldState();

	static WorldState lerpBetween(const WorldState &start, const WorldState &finish, float progress);
};

class World {
public:
	Player *human;
	Player *opponent;
	Ball *ball;

	World(SDL_Renderer *renderer, int width, int height, WorldState &worldState);
	~World();

	void startRound(WorldState &state);
	void update(WorldState &state, float timeDelta);
	void render(WorldState &state);

private:
	DISALLOW_COPY_AND_ASSIGN(World);
	int width;
	int height;
};

#endif
