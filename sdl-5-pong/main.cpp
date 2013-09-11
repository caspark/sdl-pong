#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <exception>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "util.h"
#include "main.h"

Hud::Hud(SDL_Renderer *renderer) {
	this->renderer = renderer;

	this->font = TTF_OpenFont("Vera.ttf", 24);
	if (this->font == nullptr) {
		logSDLError("TTF_OpenFont");
	}
	
	this->surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (this->surface == nullptr) {
		logSDLError("SDL_CreateRGBSurface");
	}

	this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture == nullptr) {
		logSDLError("SDL_CreateTexture");
	}
	if (SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND) != 0) {
		logSDLError("SDL_SetTextureBlendMode");
	}

	this->update();
}

void Hud::update() {
	SDL_FillRect(this->surface, nullptr, 0);

	//update the hud with new details
	SDL_Color color = { 255, 0, 0 };
	renderText("Pong - nuff said.", this->font, color, this->surface, 0, 0);
	
	//refresh the texture
	bool requiresLocking = SDL_MUSTLOCK(this->surface) != 0;

	if (requiresLocking) {
		if (SDL_LockSurface(this->surface) != 0) {
			logSDLError("SDL_LockSurface()");
		}
	}

	SDL_Rect hudSurfaceRect = { 0, 0, this->surface->w, this->surface->h };
	SDL_UpdateTexture(this->texture, &hudSurfaceRect, this->surface->pixels, this->surface->pitch);

	if (requiresLocking) {
		SDL_UnlockSurface(this->surface);
	}
}

void Hud::render() {
	renderTexture(this->texture, this->renderer, 0, 0);
}

Hud::~Hud() {
	SDL_FreeSurface(this->surface);
	SDL_DestroyTexture(this->texture);
	TTF_CloseFont(this->font);
}

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

void start_round(Player *player1, Player *player2, Ball *ball) {
	player1->pos.x = 0;
	player1->pos.y = SCREEN_HEIGHT / 2 - player1->size.y / 2;
	player2->pos.x = SCREEN_WIDTH - player2->size.x;
	player2->pos.y = SCREEN_HEIGHT / 2 - player2->size.y / 2;
	ball->pos.x = SCREEN_WIDTH / 2 - ball->size.x / 2;
	ball->pos.y = SCREEN_HEIGHT / 2 - ball->size.y / 2;
	ball->speed.x = INITIAL_BALL_X_SPEED;
	ball->speed.y = static_cast<float>(
			(rand() % ((INITIAL_BALL_Y_SPEED_MAX - INITIAL_BALL_Y_SPEED_MIN) * 2))
			- INITIAL_BALL_Y_SPEED_MAX + INITIAL_BALL_Y_SPEED_MIN
		);
}

bool rects_overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	return x1 < x2 + w2
		&& x1 + w1 > x2
		&& y1 < y2 + h2
		&& y1 + h1 > y2;
}

int main(int argc, char **argv) {
	srand((unsigned int) time(nullptr)); //seed random number generator with the current time

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError("SDL_Init");
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError("IMG_Init");
	}

	if (TTF_Init() == -1) {
		logSDLError("TTF_Init");
	}

	SDL_Window *window = SDL_CreateWindow("Pong", 600, 600, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError("CreateWindow");
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError("CreateRenderer");
	}

	Hud *hud = new Hud(renderer);

	Player *human = new Player(renderer, true);
	if (human == nullptr) {
		logFatal("Failed to load human");
	}

	Player *opponent = new Player(renderer, false);
	if (opponent == nullptr) {
		logFatal("Failed to load opponent");
	}

	Ball *ball = new Ball(renderer);

	Score score = {0, 0};
	start_round(human, opponent, ball);

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		//Read user input & handle it
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode) {
				case SDL_SCANCODE_ESCAPE:
					quit = true;
					break;
				}
				break;
			}
		}

		//handle non-event-based input
		const Uint8 *keysDown = SDL_GetKeyboardState(nullptr);
		if (keysDown[SDL_SCANCODE_UP]) {
			human->pos.y -= PLAYER_SPEED;
		} else if (keysDown[SDL_SCANCODE_DOWN]) {
			human->pos.y += PLAYER_SPEED;
		}

		//move opponent player
		float targetYPos = ball->getCenter().y;
		float targetSpeed = targetYPos - opponent->getCenter().y;
		if (targetSpeed > PLAYER_SPEED) {
			targetSpeed = PLAYER_SPEED;
		} else if (targetSpeed < -PLAYER_SPEED) {
			targetSpeed = -PLAYER_SPEED;
		}
		opponent->pos.y += targetSpeed;

		if (human->pos.y < 0) {
			human->pos.y = 0;
		} else if (human->pos.y + human->size.y > SCREEN_HEIGHT) {
			human->pos.y = SCREEN_HEIGHT - human->size.y;
		}
		if (opponent->pos.y < 0) {
			opponent->pos.y = 0;
		} else if (opponent->pos.y + opponent->size.y > SCREEN_HEIGHT) {
			opponent->pos.y = SCREEN_HEIGHT - opponent->size.y;
		}

		//Movement and collision detection
		ball->pos.x += ball->speed.x;
		ball->pos.y += ball->speed.y;

		//FIXME ball can go so fast it will move past the paddles
		if (rects_overlap(human->pos.x, human->pos.y, human->size.x, human->size.y,
				ball->pos.x + ball->speed.x, ball->pos.y, ball->size.x - ball->speed.x, ball->size.y)) {
			ball->speed.x = abs(ball->speed.x) + 1;
		}
		if (rects_overlap(opponent->pos.x, opponent->pos.y, opponent->size.x, opponent->size.y,
				ball->pos.x - ball->speed.x, ball->pos.y, ball->size.x + ball->speed.x, ball->size.y)) {
			ball->speed.x = -(abs(ball->speed.x) + 1);
		}

		if (ball->pos.y < 0 || ball->pos.y + ball->size.y > SCREEN_HEIGHT) {
			ball->speed.y *= -1;
		}

		if (ball->pos.x < 0) {
			score.opponent++;
			std::cout << "AI player wins round! Score: " << score.human 
				<< " | " << score.opponent	<< std::endl;
			start_round(human, opponent, ball);
		} else if (ball->pos.x + ball->size.x > SCREEN_WIDTH) {
			score.human++;
			std::cout << "Human player wins round!" << score.human 
				<< " | " << score.opponent	<< std::endl;
			start_round(human, opponent, ball);
		}

		//Render our scene
		SDL_RenderClear(renderer);

		human->render();
		opponent->render();
		ball->render();
		
		hud->render();

		SDL_RenderPresent(renderer);
	}

	std::cout << "Quitting" << std::endl;

	//cleanup
	delete human;
	delete opponent;
	delete ball;
	delete hud;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}