#include <string>
#include <iostream>
#include <time.h>
#include <exception>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "world.h"
#include "entities.h"
#include "util.h"

const int PLAYER_SPEED = 5;
const int INITIAL_BALL_X_SPEED = 5;
const int INITIAL_BALL_Y_SPEED_MIN = 2;
const int INITIAL_BALL_Y_SPEED_MAX = 5;

World::World(SDL_Renderer *renderer, int width, int height) {
	SDL_assert(renderer != nullptr);
	this->width = width;
	this->height = height;

	this->human = new Player(renderer, true);
	this->opponent = new Player(renderer, false);
	this->ball = new Ball(renderer);
}

World::~World() {
	delete this->human;
	delete this->opponent;
	delete this->ball;
}

void World::startRound() {
	this->human->pos.x = 0;
	this->human->pos.y = height / 2 - this->human->size.y / 2;
	this->opponent->pos.x = width - this->opponent->size.x;
	this->opponent->pos.y = height / 2 - this->opponent->size.y / 2;
	this->ball->pos.x = width / 2 - this->ball->size.x / 2;
	this->ball->pos.y = height / 2 - this->ball->size.y / 2;
	this->ball->speed.x = INITIAL_BALL_X_SPEED;
	this->ball->speed.y = static_cast<float>(
			(rand() % ((INITIAL_BALL_Y_SPEED_MAX - INITIAL_BALL_Y_SPEED_MIN) * 2))
			- INITIAL_BALL_Y_SPEED_MAX + INITIAL_BALL_Y_SPEED_MIN
		);
}

void World::update() {
	//handle non-event-based input
	const Uint8 *keysDown = SDL_GetKeyboardState(nullptr);
	if (keysDown[SDL_SCANCODE_UP]) {
		this->human->pos.y -= PLAYER_SPEED;
	} else if (keysDown[SDL_SCANCODE_DOWN]) {
		this->human->pos.y += PLAYER_SPEED;
	}

	//move this->opponent player
	float targetYPos = this->ball->getCenter().y;
	float targetSpeed = targetYPos - this->opponent->getCenter().y;
	if (targetSpeed > PLAYER_SPEED) {
		targetSpeed = PLAYER_SPEED;
	} else if (targetSpeed < -PLAYER_SPEED) {
		targetSpeed = -PLAYER_SPEED;
	}
	this->opponent->pos.y += targetSpeed;

	if (this->human->pos.y < 0) {
		this->human->pos.y = 0;
	} else if (this->human->pos.y + this->human->size.y > height) {
		this->human->pos.y = height - this->human->size.y;
	}
	if (this->opponent->pos.y < 0) {
		this->opponent->pos.y = 0;
	} else if (this->opponent->pos.y + this->opponent->size.y > height) {
		this->opponent->pos.y = height - this->opponent->size.y;
	}

	//Movement and collision detection
	this->ball->pos.x += this->ball->speed.x;
	this->ball->pos.y += this->ball->speed.y;

	//FIXME this->ball can go so fast it will move past the paddles
	if (rects_overlap(this->human->pos.x, this->human->pos.y, this->human->size.x, this->human->size.y,
			this->ball->pos.x + this->ball->speed.x, this->ball->pos.y,
			this->ball->size.x - this->ball->speed.x, this->ball->size.y)) {
		this->ball->speed.x = abs(this->ball->speed.x) + 1;
	}
	if (rects_overlap(this->opponent->pos.x, this->opponent->pos.y, this->opponent->size.x, this->opponent->size.y,
			this->ball->pos.x - this->ball->speed.x, this->ball->pos.y,
			this->ball->size.x + this->ball->speed.x, this->ball->size.y)) {
		this->ball->speed.x = -(abs(this->ball->speed.x) + 1);
	}

	if (this->ball->pos.y < 0 || this->ball->pos.y + this->ball->size.y > height) {
		this->ball->speed.y *= -1;
	}

	if (this->ball->pos.x < 0) {
		++this->opponent->score;
		std::cout << "AI player wins round! Score: " << this->human->score 
			<< " | " << this->opponent->score << std::endl;
		startRound();
	} else if (this->ball->pos.x + this->ball->size.x > width) {
		++this->human->score;
		std::cout << "Human player wins round! Score: " << this->human->score 
			<< " | " << this->opponent->score << std::endl;
		startRound();
	}
}

void World::render() {
	this->human->render();
	this->opponent->render();
	this->ball->render();
}