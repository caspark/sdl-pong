#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "util.h"
#include "main.h"

void hud_update(Hud *hud) {
	SDL_FillRect(hud->surface, nullptr, 0);

	//update the hud with new details
	SDL_Color color = { 255, 0, 0 };
	renderText("Pong - nuff said.", hud->font, color, hud->surface, 0, 0);
	
	//refresh the texture
	bool requiresLocking = SDL_MUSTLOCK(hud->surface) != 0;

	if (requiresLocking) {
		if (SDL_LockSurface(hud->surface) != 0) {
			logSDLError("SDL_LockSurface()");
		}
	}

	SDL_Rect hudSurfaceRect = { 0, 0, hud->surface->w, hud->surface->h };
	SDL_UpdateTexture(hud->texture, &hudSurfaceRect, hud->surface->pixels, hud->surface->pitch);

	if (requiresLocking) {
		SDL_UnlockSurface(hud->surface);
	}
}

Hud *hud_load(SDL_Renderer *renderer) {
	Hud *hud = new Hud;

	hud->font = TTF_OpenFont("Vera.ttf", 24);
	if (hud->font == nullptr) {
		logSDLError("TTF_OpenFont");
	}
	
	hud->surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (hud->surface == nullptr) {
		logSDLError("SDL_CreateRGBSurface");
	}

	hud->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (hud->texture == nullptr) {
		logSDLError("SDL_CreateTexture");
	}
	if (SDL_SetTextureBlendMode(hud->texture, SDL_BLENDMODE_BLEND) != 0) {
		logSDLError("SDL_SetTextureBlendMode");
	}

	hud_update(hud);

	return hud;
}

void hud_render(Hud *hud, SDL_Renderer *renderer) {
	renderTexture(hud->texture, renderer, 0, 0);
}

void hud_free(Hud *hud) {
	SDL_FreeSurface(hud->surface);
	SDL_DestroyTexture(hud->texture);
	TTF_CloseFont(hud->font);
	delete hud;
}

Player* player_load(SDL_Renderer *renderer, bool isLeft) {
	Player *player = new Player;
	player->tex = loadTexture("paddle.png", renderer);
	if (player->tex == nullptr) {
		return nullptr;
	}
	int w, h;
	SDL_QueryTexture(player->tex, nullptr, nullptr, &w, &h);
	player->size.x = static_cast<float>(w);
	player->size.y = static_cast<float>(h);
	return player;
}

void player_free(Player *player) {
	SDL_DestroyTexture(player->tex);
	delete player;
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

Ball* ball_load(SDL_Renderer *renderer) {
	Ball *ball = new Ball;
	ball->tex = loadTexture("ball.png", renderer);
	if (ball->tex == nullptr) {
		return nullptr;
	}
	int w, h;
	SDL_QueryTexture(ball->tex, nullptr, nullptr, &w, &h);
	ball->size.x = static_cast<float>(w);
	ball->size.y = static_cast<float>(h);
	return ball;
}

void ball_free(Ball *ball) {
	SDL_DestroyTexture(ball->tex);
	delete ball;
}

bool rects_overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	return x1 < x2 + w2
		&& x1 + w1 > x2
		&& y1 < y2 + h2
		&& y1 + h1 > y2;
}

Vector2 getCenter(Vector2 pos, Vector2 size) {
	Vector2 center = {pos.x + size.x / 2, pos.y + size.y / 2};
	return center;
}

Vector2 getCenter(Player *player) {
	return getCenter(player->pos, player->size);
}

Vector2 getCenter(Ball *ball) {
	return getCenter(ball->pos, ball->size);
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

	Hud *hud = hud_load(renderer);

	Player *human = player_load(renderer, true);
	if (human == nullptr) {
		logFatal("Failed to load human");
	}

	Player *opponent = player_load(renderer, false);
	if (opponent == nullptr) {
		logFatal("Failed to load opponent");
	}

	Ball *ball = ball_load(renderer);

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
		int targetYPos = getCenter(ball).y;
		int targetSpeed = targetYPos - getCenter(opponent).y;
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

		renderTexture(human->tex, renderer, human->pos.x, human->pos.y);
		renderTexture(opponent->tex, renderer, opponent->pos.x, opponent->pos.y);
		renderTexture(ball->tex, renderer, ball->pos.x, ball->pos.y);
		
		hud_render(hud, renderer);

		SDL_RenderPresent(renderer);
	}

	std::cout << "Quitting" << std::endl;

	//cleanup
	player_free(human);
	player_free(opponent);
	ball_free(ball);
	hud_free(hud);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}