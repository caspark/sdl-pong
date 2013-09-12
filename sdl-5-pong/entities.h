#ifndef ENTITIES_H
#define ENTITIES_H

#include <SDL.h>

#include "util.h"

typedef struct Vector2_struct {
	float x;
	float y;
} Vector2;

class Player {
public:
	Vector2 pos;
	Vector2 size;
	int score;

	Player(SDL_Renderer *renderer, bool isLeft);
	~Player();

	void render();

	Vector2 getCenter();

private:
	DISALLOW_COPY_AND_ASSIGN(Player);
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

class Ball {
public:
	Vector2 pos;
	Vector2 speed;
	Vector2 size;

	Ball(SDL_Renderer *renderer);
	~Ball();

	void render();

	Vector2 getCenter();

private:
	DISALLOW_COPY_AND_ASSIGN(Ball);
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

#endif
