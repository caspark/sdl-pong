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

const float PLAYER_SPEED = 7 / PHYSICS_TIMESTEP;
const float INITIAL_BALL_X_SPEED = 5 / PHYSICS_TIMESTEP;
const int INITIAL_BALL_Y_SPEED_MIN = static_cast<int>(2 / PHYSICS_TIMESTEP);
const int INITIAL_BALL_Y_SPEED_MAX = static_cast<int>(5 / PHYSICS_TIMESTEP);

WorldState::WorldState() {
	this->human = MovingRect();
	this->humanScore = 0;
	this->opponent = MovingRect();
	this->opponentScore = 0;
	this->ball = MovingRect();
}

WorldState WorldState::lerpBetween(const WorldState &start, const WorldState &finish, float progress) {
	WorldState lerped;
	lerped.human = MovingRect::lerpBetween(start.human, finish.human, progress);
	lerped.opponent = MovingRect::lerpBetween(start.opponent, finish.opponent, progress);
	lerped.ball = MovingRect::lerpBetween(start.ball, finish.ball, progress);
	return lerped;
}

World::World(SDL_Renderer *renderer, int width, int height, WorldState &worldState) {
	SDL_assert(renderer != nullptr);
	this->width = width;
	this->height = height;

	this->human = new Player(renderer, worldState.human);
	this->opponent = new Player(renderer, worldState.opponent);
	this->ball = new Ball(renderer, worldState.ball);
}

World::~World() {
	delete this->human;
	delete this->opponent;
	delete this->ball;
}

void World::startRound(WorldState &state) {
	state.human.pos.x = 0;
	state.human.pos.y = height / 2 - state.human.size.y / 2;

	state.opponent.pos.x = width - state.opponent.size.x;
	state.opponent.pos.y = height / 2 - state.opponent.size.y / 2;

	state.ball.pos.x = width / 2 - state.ball.size.x / 2;
	state.ball.pos.y = height / 2 - state.ball.size.y / 2;
	state.ball.speed.x = INITIAL_BALL_X_SPEED;
	state.ball.speed.y = static_cast<float>(
			randomIntInRange(INITIAL_BALL_Y_SPEED_MIN, INITIAL_BALL_Y_SPEED_MAX) * randomSignForInt()
		);
}

void World::update(WorldState &state, float timeDelta) {
	//handle non-event-based input
	const Uint8 *keysDown = SDL_GetKeyboardState(nullptr);
	if (keysDown[SDL_SCANCODE_UP]) {
		state.human.speed.y = -PLAYER_SPEED;
	} else if (keysDown[SDL_SCANCODE_DOWN]) {
		state.human.speed.y = PLAYER_SPEED;
	} else {
		state.human.speed.y = 0;
	}

	//"ai" for opponent player
	float aiIdealDistanceToCover = state.ball.getCenter().y - state.opponent.getCenter().y;
	if (aiIdealDistanceToCover > PLAYER_SPEED * timeDelta) {
		state.opponent.speed.y = PLAYER_SPEED;
	} else if (aiIdealDistanceToCover < -PLAYER_SPEED * timeDelta) {
		state.opponent.speed.y = -PLAYER_SPEED;
	} else {
		state.opponent.speed.y = 0;
	}

	//simulate
	state.human.pos.y += state.human.speed.y * timeDelta;
	state.opponent.pos.y += state.opponent.speed.y * timeDelta;
	state.ball.pos.x += state.ball.speed.x * timeDelta;
	state.ball.pos.y += state.ball.speed.y * timeDelta;

	//fixup

	//FIXME ball can go so fast it will move past the paddles
	if (rects_overlap(state.human.pos.x, state.human.pos.y, state.human.size.x, state.human.size.y,
			state.ball.pos.x, state.ball.pos.y,
			state.ball.size.x * 2, state.ball.size.y)) {
		state.ball.speed.x = abs(state.ball.speed.x) * 1.1f;
		state.ball.pos.x = state.human.pos.x + state.human.size.x;
		std::cout << "Paddle collision (HUMAN) - ball speed is now " << state.ball.speed.x << std::endl;
	}
	if (rects_overlap(state.opponent.pos.x, state.opponent.pos.y, state.opponent.size.x, state.opponent.size.y,
			state.ball.pos.x - state.ball.size.x, state.ball.pos.y,
			state.ball.size.x * 2, state.ball.size.y)) {
		state.ball.speed.x = -abs(state.ball.speed.x) * 1.1f;
		state.ball.pos.x = state.opponent.pos.x - state.ball.size.x;
		std::cout << "Paddle collision (OPPON) - ball speed is now " << state.ball.speed.x << std::endl;
	}

	if (state.ball.pos.y < 0) {
		state.ball.speed.y *= -1;
		state.ball.pos.y = 0;
	} else if (state.ball.pos.y + state.ball.size.y > this->height) {
		state.ball.speed.y *= -1;
		state.ball.pos.y = this->height - state.ball.size.y;
	}

	if (state.human.pos.y < 0) {
		state.human.pos.y = 0;
	} else if (state.human.pos.y + state.human.size.y > this->height) {
		state.human.pos.y = this->height - state.human.size.y;
	}
	if (state.opponent.pos.y < 0) {
		state.opponent.pos.y = 0;
	} else if (state.opponent.pos.y + state.opponent.size.y > this->height) {
		state.opponent.pos.y = this->height - state.opponent.size.y;
	}

	if (state.ball.pos.x + state.ball.size.x < 0) {
		++state.opponentScore;
		std::cout << "AI player wins round! Score: " << state.humanScore 
			<< " | " << state.opponentScore << std::endl;
		startRound(state);
	} else if (state.ball.pos.x > width) {
		++state.humanScore;
		std::cout << "Human player wins round! Score: " << state.humanScore 
			<< " | " << state.opponentScore << std::endl;
		startRound(state);
	}
}

void World::render(WorldState &state) {
	this->human->render(state.human);
	this->opponent->render(state.opponent);
	this->ball->render(state.ball);
}