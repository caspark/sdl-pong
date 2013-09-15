#include <string>
#include <iostream>

#include "util.h"

//adapted from SDL_assert.c
#ifdef __WIN32__
#include <windows.h> //Changed: including windows.h directly instead of SDL_windows.h

#ifndef WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW 0
#endif
#else  /* fprintf, _exit(), etc. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

void handleFatal(int exitcode) {
	std::cerr << "Fatal error encountered; waiting then quitting" << std::endl;
	SDL_Delay(3000);
	SDL_Quit();

//stolen from SDL_assert.c
#ifdef __WIN32__
    ExitProcess(exitcode);
#else
    _exit(exitcode);
#endif
}

void logSDLError(const std::string &sdlFunctionName) {
	std::cerr << "SDL Error: " << sdlFunctionName << ": " << SDL_GetError() << std::endl;
	handleFatal(541); // 541 is sort of close to SDL
}

void logFatal(const std::string &msg) {
	std::cerr << "Fatal: " << msg << std::endl;
	handleFatal(4532); // 4532 is as closer to USER as we're going to get
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr) {
		logSDLError("IMG_LoadTexture");
	}
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	if (SDL_RenderCopy(ren, tex, nullptr, &dst) != 0) {
		logSDLError("SDL_RenderCopy()");
	}
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
	int w, h;
	SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, float x, float y) {
	renderTexture(tex, ren, static_cast<int>(x), static_cast<int>(y));
}

bool rects_overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	return x1 < x2 + w2
		&& x1 + w1 > x2
		&& y1 < y2 + h2
		&& y1 + h1 > y2;
}

int randomIntInRange(int min, int max) {
	// dealing with rand()'s inadequacies as per http://eternallyconfuzzled.com/arts/jsw_art_rand.aspx
	double uniformDeviate = rand() * (1.0 / (RAND_MAX + 1.0));
	// the +1 makes this a closed rather than half open range
	int generatedRandom = static_cast<int>(min + uniformDeviate * (max + 1 - min));
	SDL_assert(generatedRandom >= min);
	SDL_assert(generatedRandom <= max);
	return generatedRandom;
}

int randomSignForInt() {
	return randomIntInRange(0, 1) * 2 - 1;
}

FpsTracker::FpsTracker(int numberOfSamples) {
	this->frameIndex = 0;
	this->totalFrameTime = 0;
	this->frameTimes = new float[numberOfSamples];
	this->numberOfSamples = numberOfSamples;
	this->samplesSoFar = 0;
}

FpsTracker::~FpsTracker() {
	delete[] frameTimes;
}

float FpsTracker::calculateAverageFrameTime(float frameTime) {
	if (this->samplesSoFar < this->numberOfSamples) {
		this->samplesSoFar += 1;
	} else {
		this->totalFrameTime -= this->frameTimes[this->frameIndex];
	}
	this->totalFrameTime += frameTime;
	this->frameTimes[this->frameIndex] = frameTime;
	this->frameIndex += 1;
	if (this->frameIndex == this->numberOfSamples) {
		this->frameIndex = 0;
	}

	return this->totalFrameTime/this->samplesSoFar;
}
