#include <SDL.h>
#include <SDL_TTF.h>

#include "util.h"
#include "hud.h"

Hud::Hud(SDL_Renderer *renderer, int screenWidth, int screenHeight) {
	this->renderer = renderer;

	this->font = TTF_OpenFont("Vera.ttf", 24);
	if (this->font == nullptr) {
		logSDLError("TTF_OpenFont");
	}
	
	this->surface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (this->surface == nullptr) {
		logSDLError("SDL_CreateRGBSurface");
	}

	this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
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