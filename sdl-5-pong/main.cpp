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

PLAYER* player_load(SDL_Renderer *renderer, bool isLeft) {
	PLAYER *player = new PLAYER;
	player->tex = loadTexture("paddle.png", renderer);
	if (player->tex == nullptr) {
		return nullptr;
	}
	SDL_QueryTexture(player->tex, nullptr, nullptr, &(player->size.x), &(player->size.y));
	return player;
}

void player_free(PLAYER *player) {
	SDL_DestroyTexture(player->tex);
	delete player;
}

void start_round(PLAYER *player1, PLAYER *player2, BALL *ball) {
	player1->pos.x = 0;
	player1->pos.y = SCREEN_HEIGHT / 2 - player1->size.y / 2;
	player2->pos.x = SCREEN_WIDTH - player2->size.x;
	player2->pos.y = SCREEN_HEIGHT / 2 - player2->size.y / 2;
	ball->pos.x = SCREEN_WIDTH / 2 - ball->size.x / 2;
	ball->pos.y = SCREEN_HEIGHT / 2 - ball->size.y / 2;
	ball->speed.x = INITIAL_BALL_X_SPEED;
	ball->speed.y = (rand() % ((INITIAL_BALL_Y_SPEED_MAX - INITIAL_BALL_Y_SPEED_MIN) * 2))
		- INITIAL_BALL_Y_SPEED_MAX + INITIAL_BALL_Y_SPEED_MIN;
}

BALL* ball_load(SDL_Renderer *renderer) {
	BALL *ball = new BALL;
	ball->tex = loadTexture("ball.png", renderer);
	if (ball->tex == nullptr) {
		return nullptr;
	}
	SDL_QueryTexture(ball->tex, nullptr, nullptr, &(ball->size.x), &(ball->size.y));
	return ball;
}

void ball_free(BALL *ball) {
	SDL_DestroyTexture(ball->tex);
	delete ball;
}

bool rects_overlap(VEC2 p1, VEC2 s1, VEC2 p2, VEC2 s2) {
	return p1.x < p2.x + s2.x
		&& p1.x + s1.x > p2.x
		&& p1.y < p2.y + s2.y
		&& p1.y + s1.y > p2.y;
}

VEC2 getCenter(VEC2 pos, VEC2 size) {
	VEC2 center = {pos.x + size.x / 2, pos.y + size.y / 2};
	return center;
}

VEC2 getCenter(PLAYER *player) {
	return getCenter(player->pos, player->size);
}

VEC2 getCenter(BALL *ball) {
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

	PLAYER *human = player_load(renderer, true);
	if (human == nullptr) {
		logFatal("Failed to load human");
	}

	PLAYER *opponent = player_load(renderer, false);
	if (opponent == nullptr) {
		logFatal("Failed to load opponent");
	}

	BALL *ball = ball_load(renderer);

	SCORE score = {0, 0};
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

		//Movement and collision detection
		ball->pos.x += ball->speed.x;
		ball->pos.y += ball->speed.y;

		//FIXME ball can go so fast it will move past the paddles
		if (rects_overlap(human->pos, human->size, ball->pos, ball->size)) {
			ball->speed.x *= -1;
			ball->speed.x += ((ball->speed.x > 0) - (ball->speed.x < 0)) * 1;
		}
		if (rects_overlap(opponent->pos, opponent->size, ball->pos, ball->size)) {
			ball->speed.x *= -1;
			ball->speed.x += ((ball->speed.x > 0) - (ball->speed.x < 0)) * 1;
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