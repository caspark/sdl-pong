#ifndef ENTITIES_H
#define ENTITIES_H

#include <SDL.h>

typedef struct Vector2_struct {
	float x;
	float y;
} Vector2;

class Player {
public:
	Vector2 pos;
	Vector2 size;

	Player(SDL_Renderer *renderer, bool isLeft);
	~Player();

	void render();

	Vector2 getCenter();

private:
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
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

#endif
