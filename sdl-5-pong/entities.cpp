#include "entities.h"
#include "util.h"

Vector2 getCenter(Vector2 pos, Vector2 size) {
	Vector2 center = {pos.x + size.x / 2, pos.y + size.y / 2};
	return center;
}

Player::Player(SDL_Renderer *renderer, bool isLeft) {
	this->renderer = renderer;
	this->tex = loadTexture("paddle.png", renderer);
	SDL_assert(this->tex != nullptr);
	int w, h;
	SDL_assert(SDL_QueryTexture(this->tex, nullptr, nullptr, &w, &h) == 0);
	this->size.x = static_cast<float>(w);
	this->size.y = static_cast<float>(h);
}

Player::~Player() {
	SDL_DestroyTexture(tex);
}

void Player::render() {
	renderTexture(this->tex, this->renderer, this->pos.x, this->pos.y);
}

Vector2 Player::getCenter() {
	return ::getCenter(this->pos, this->size);
}

Ball::Ball(SDL_Renderer *renderer) {
	this->renderer = renderer;
	this->tex = loadTexture("ball.png", renderer);
	SDL_assert(this->tex != nullptr);
	int w, h;
	SDL_assert(SDL_QueryTexture(this->tex, nullptr, nullptr, &w, &h) == 0);
	this->size.x = static_cast<float>(w);
	this->size.y = static_cast<float>(h);
}

Ball::~Ball() {
	SDL_DestroyTexture(this->tex);
}

void Ball::render() {
	renderTexture(this->tex, renderer, this->pos.x, this->pos.y);
}

Vector2 Ball::getCenter() {
	return ::getCenter(this->pos, this->size);
}