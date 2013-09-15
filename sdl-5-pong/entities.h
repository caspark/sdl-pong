#ifndef ENTITIES_H
#define ENTITIES_H

#include <SDL.h>

#include "util.h"

class Vector2 {
public:
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
};

class MovingRect {
public:
	Vector2 pos;
	Vector2 speed;
	Vector2 size;

	MovingRect();

	Vector2 getCenter();

	static MovingRect lerpBetween(const MovingRect &start, const MovingRect &finish, float progress);
};

class Player {
public:
	int score;

	Player(SDL_Renderer *renderer, MovingRect &state);
	~Player();

	void render(MovingRect &state);

private:
	DISALLOW_COPY_AND_ASSIGN(Player);
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

class Ball {
public:
	Ball(SDL_Renderer *renderer, MovingRect &state);
	~Ball();

	void render(MovingRect &state);

private:
	DISALLOW_COPY_AND_ASSIGN(Ball);
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

#endif
