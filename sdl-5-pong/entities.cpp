#include "entities.h"
#include "util.h"

Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

MovingRect::MovingRect() {
	this->pos = Vector2();
	this->size = Vector2();
	this->speed = Vector2();
}

MovingRect MovingRect::lerpBetween(const MovingRect &start, const MovingRect &finish, float progress) {
	MovingRect lerped;
	lerped.pos.x = start.pos.x * (1 - progress) + finish.pos.x * progress;
	lerped.pos.y = start.pos.y * (1 - progress) + finish.pos.y * progress;
	return lerped;
}

Vector2 getCenter(Vector2 pos, Vector2 size) {
	return Vector2(pos.x + size.x / 2, pos.y + size.y / 2);
}

Vector2 MovingRect::getCenter() {
	return ::getCenter(this->pos, this->size);
}

Player::Player(SDL_Renderer *renderer, MovingRect &state) {
	this->renderer = renderer;
	this->tex = loadTexture("paddle.png", renderer);
	SDL_assert(this->tex != nullptr);
	int w, h;
	SDL_assert(SDL_QueryTexture(this->tex, nullptr, nullptr, &w, &h) == 0);
	state.size.x = static_cast<float>(w);
	state.size.y = static_cast<float>(h);
	state.speed.x = 0;
	state.speed.y = 0;
	this->score = 0;
}

Player::~Player() {
	SDL_DestroyTexture(tex);
}

void Player::render(MovingRect &state) {
	renderTexture(this->tex, this->renderer, state.pos.x, state.pos.y);
}

Ball::Ball(SDL_Renderer *renderer, MovingRect &state) {
	this->renderer = renderer;
	this->tex = loadTexture("ball.png", renderer);
	SDL_assert(this->tex != nullptr);
	int w, h;
	SDL_assert(SDL_QueryTexture(this->tex, nullptr, nullptr, &w, &h) == 0);
	state.size.x = static_cast<float>(w);
	state.size.y = static_cast<float>(h);
}

Ball::~Ball() {
	SDL_DestroyTexture(this->tex);
}

void Ball::render(MovingRect &ball) {
	renderTexture(this->tex, renderer, ball.pos.x, ball.pos.y);
}