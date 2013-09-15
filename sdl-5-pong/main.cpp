#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <exception>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "util.h"
#include "entities.h"
#include "hud.h"
#include "world.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

struct State
{
	float x;
	float v;
};

struct Derivative
{
	float dx;
	float dv;
};

State interpolate(const State &previous, const State &current, float alpha)
{
	State state;
	state.x = current.x*alpha + previous.x*(1-alpha);
	state.v = current.v*alpha + previous.v*(1-alpha);
	return state;
}

float acceleration(const State &state, float t)
{
	const float k = 10;
	const float b = 1;
	return - k*state.x - b*state.v;
}

Derivative evaluate(const State &initial, float t)
{
	Derivative output;
	output.dx = initial.v;
	output.dv = acceleration(initial, t);
	return output;
}

Derivative evaluate(const State &initial, float t, float dt, const Derivative &d)
{
	State state;
	state.x = initial.x + d.dx*dt;
	state.v = initial.v + d.dv*dt;
	Derivative output;
	output.dx = state.v;
	output.dv = acceleration(state, t+dt);
	return output;
}

void integrate(State &state, float t, float dt)
{
	Derivative a = evaluate(state, t);
	Derivative b = evaluate(state, t, dt*0.5f, a);
	Derivative c = evaluate(state, t, dt*0.5f, b);
	Derivative d = evaluate(state, t, dt, c);
	
	const float dxdt = 1.0f/6.0f * (a.dx + 2.0f*(b.dx + c.dx) + d.dx);
	const float dvdt = 1.0f/6.0f * (a.dv + 2.0f*(b.dv + c.dv) + d.dv);
	
	state.x = state.x + dxdt*dt;
	state.v = state.v + dvdt*dt;
}

void drawUI(Hud *hud, WorldState &state) {
	hud->setTextColor(255, 0, 0);
	hud->drawTextBlended(SCREEN_WIDTH / 2, 0, "It's a Pong!", AlignH::Center);

	hud->setTextColor(0, 0, 255);
	hud->drawTextBlended(0, SCREEN_HEIGHT, std::to_string(state.humanScore).c_str(),
		AlignH::Left, AlignV::Bottom);
	hud->drawTextBlended(SCREEN_WIDTH, SCREEN_HEIGHT, std::to_string(state.opponentScore).c_str(), AlignH::Right, AlignV::Bottom);
}

void drawFps(Hud *hud, int fps) {
	hud->setTextColor(255, 176, 0);
	std::stringstream ss;
	ss << "FPS: " << fps;
	hud->drawTextFast(SCREEN_WIDTH, 0, ss.str().c_str(), AlignH::Right);
}

int main(int argc, char **argv) {
	srand(static_cast<unsigned int>(time(nullptr))); //seed random number generator with the current time

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError("SDL_Init");
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError("IMG_Init");
	}

	if (TTF_Init() == -1) {
		logSDLError("TTF_Init");
	}

	SDL_Window *window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError("CreateWindow");
	}
	//SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		logSDLError("CreateRenderer");
	}

	FpsTracker fpsTracker(100);

	WorldState currentWorldState;
	World *world = new World(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, currentWorldState);
	world->startRound(currentWorldState);
	WorldState previousWorldState=currentWorldState;

	Hud *hud = new Hud(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	drawUI(hud, currentWorldState);

	State current;
	current.x = 100;
	current.v = 0;
	
	State previous = current;

	float t = 0.0f;
	float dt = PHYSICS_TIMESTEP;

	Uint64 currentTime = SDL_GetPerformanceCounter();
	float accumulator = 0;

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		const Uint64 newTime = SDL_GetPerformanceCounter();
		float deltaTime = static_cast<float>(newTime - currentTime) / SDL_GetPerformanceFrequency(); //aka time for this frame
		currentTime = newTime;

		if (deltaTime > 0.25f) {
			std::cout << "limiting delta time to 0.25f" << std::endl;
			deltaTime = 0.25f; // anti "spiral of death" / breakpoints
		}

		accumulator += deltaTime;

		int simCount = 0;
		while (accumulator >= dt) {
			accumulator -= dt;
			previous = current;
			integrate(current, t, dt);
			previousWorldState = currentWorldState;
			world->update(currentWorldState, dt); //aka integrate
			if (currentWorldState.humanScore != previousWorldState.humanScore
				|| currentWorldState.opponentScore != previousWorldState.opponentScore) {
					drawUI(hud, currentWorldState);
			}
			t += dt;
			++simCount;
		}
		if (simCount > 1) {
			std::cout << "Simulated multiple steps:" << simCount << std::endl;
		}

		State state = interpolate(previous, current, accumulator/dt);
		WorldState lerped = WorldState::lerpBetween(previousWorldState, currentWorldState, accumulator/dt);

		drawFps(hud, static_cast<int>(1 / fpsTracker.calculateAverageFrameTime(deltaTime)));

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		world->render(lerped);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
		SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2,
			static_cast<int>(SCREEN_WIDTH / 2 + state.x * 3), SCREEN_HEIGHT / 2);
		hud->render();
		SDL_RenderPresent(renderer);

		//TODO figure out where user input handling should go. @see http://gamedev.stackexchange.com/questions/8623/a-good-way-to-build-a-game-loop-in-opengl
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
	}

	std::cout << "Quitting" << std::endl;

	//cleanup
	delete hud;
	delete world;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
