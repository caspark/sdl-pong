#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

const int PLAYER_SPEED = 5;
const int INITIAL_BALL_X_SPEED = 5;
const int INITIAL_BALL_Y_SPEED_MIN = 1;
const int INITIAL_BALL_Y_SPEED_MAX = 4;

/**
* Log an SDL error with some error message to the output stream of our choice,
* then sleep for a bit.
* @param os The output stream to write the message too
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logSDLError(std::ostream &stream, const std::string &msg) {
	stream << "Error: " << msg << ": " << SDL_GetError() << std::endl;
	SDL_Delay(2000);
}

/**
* Log an SDL error with some error message to the output stream of our choice,
* then sleep for a bit.
* @param os The output stream to write the message too
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logFatal(std::ostream &stream, const std::string &msg) {
	stream << "Fatal: " << msg << std::endl;
	SDL_Delay(2000);
}

/**
* Loads an image into a texture on the rendering device
* @param file The image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr) {
		logSDLError(std::cout, "IMG_LoadTexture");
	}
	return texture;
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw too
* @param x The x coordinate to draw too
* @param y The y coordinate to draw too
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw too
* @param x The x coordinate to draw too
* @param y The y coordinate to draw too
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

typedef struct {
	int x;
	int y;
} VEC2;

typedef struct {
	VEC2 pos;
	VEC2 size;
	SDL_Texture *tex;
} PLAYER;

typedef struct {
	VEC2 pos;
	VEC2 speed;
	VEC2 size;
	SDL_Texture *tex;
} BALL;

PLAYER* player_load(SDL_Renderer *renderer, bool isLeft) {
	PLAYER *player = new PLAYER;
	player->tex = loadTexture("paddle.png", renderer);
	if (player->tex == nullptr) {
		return nullptr;
	}
	SDL_QueryTexture(player->tex, nullptr, nullptr, &(player->size.x), &(player->size.y));
	if (isLeft) {
		player->pos.x = 0;
		player->pos.y = SCREEN_HEIGHT / 2 - player->size.y / 2;
	} else {
		player->pos.x = SCREEN_WIDTH - player->size.x;
		player->pos.y = SCREEN_HEIGHT / 2 - player->size.y / 2;
	}
	return player;
}

void player_free(PLAYER *player) {
	SDL_DestroyTexture(player->tex);
	delete player;
}

BALL* ball_load(SDL_Renderer *renderer) {
	BALL *ball = new BALL;
	ball->tex = loadTexture("ball.png", renderer);
	if (ball->tex == nullptr) {
		return nullptr;
	}
	SDL_QueryTexture(ball->tex, nullptr, nullptr, &(ball->size.x), &(ball->size.y));
	ball->pos.x = SCREEN_WIDTH / 2 - ball->size.x / 2;
	ball->pos.y = SCREEN_HEIGHT / 2 - ball->size.y / 2;
	ball->speed.x = INITIAL_BALL_X_SPEED;
	ball->speed.y = (rand() % ((INITIAL_BALL_Y_SPEED_MAX - INITIAL_BALL_Y_SPEED_MIN) * 2))
		- INITIAL_BALL_Y_SPEED_MAX + INITIAL_BALL_Y_SPEED_MIN;
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

int main(int argc, char **argv){
	srand(time(NULL)); //seed random number generator with the current time

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		logSDLError(std::cout, "IMG_Init");
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Pong", 600, 600, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		return 2;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		return 3;
	}

	PLAYER *human = player_load(renderer, true);
	if (human == nullptr) {
		logFatal(std::cout, "Failed to load human");
		return 6;
	}

	PLAYER *opponent = player_load(renderer, false);
	if (opponent == nullptr) {
		logFatal(std::cout, "Failed to load opponent");
		return 6;
	}

	BALL *ball = ball_load(renderer);

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

		//Render our scene
		SDL_RenderClear(renderer);

		renderTexture(human->tex, renderer, human->pos.x, human->pos.y);
		renderTexture(opponent->tex, renderer, opponent->pos.x, opponent->pos.y);
		renderTexture(ball->tex, renderer, ball->pos.x, ball->pos.y);

		SDL_RenderPresent(renderer);
	}

	std::cout << "All done, exiting" << std::endl;

	//cleanup
	player_free(human);
	player_free(opponent);
	ball_free(ball);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return 0;
}