#include <SDL.h>
#include <SDL_TTF.h>

#include "util.h"
#include "hud.h"

Hud::Hud(SDL_Renderer *renderer, int screenWidth, int screenHeight) {
	this->renderer = renderer;
	SDL_Color defaultColor = {255, 255, 255};
	this->color = color;

	this->font = TTF_OpenFont("Vera.ttf", 24);
	if (this->font == nullptr) {
		logSDLError("TTF_OpenFont");
	}
	
	this->slowSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (this->slowSurface == nullptr) {
		logSDLError("SDL_CreateRGBSurface");
	}

	this->slowSurfaceDirty = false;

	this->slowTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
	if (this->slowTexture == nullptr) {
		logSDLError("SDL_CreateTexture");
	}
	if (SDL_SetTextureBlendMode(this->slowTexture, SDL_BLENDMODE_BLEND) != 0) {
		logSDLError("SDL_SetTextureBlendMode");
	}

	this->fastSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (this->slowSurface == nullptr) {
		logSDLError("SDL_CreateRGBSurface");
	}

	this->fastSurfaceDirty = false;

	this->fastTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
	if (this->fastTexture == nullptr) {
		logSDLError("SDL_CreateTexture");
	}
	if (SDL_SetTextureBlendMode(this->fastTexture, SDL_BLENDMODE_BLEND) != 0) {
		logSDLError("SDL_SetTextureBlendMode");
	}
}

void Hud::setTextColor(uint8_t r, uint8_t g, uint8_t b) {
	SDL_Color newColor = {r, g, b};
	this->color = newColor;
}

void Hud::drawTextFast(int x, int y, const char *text) {
	drawTextFast(x, y, text, AlignH::Left);
}

void Hud::drawTextFast(int x, int y, const char *text, AlignH align) {
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, this->color);
	if (textSurface == nullptr) {
		logSDLError("TTF_RenderText_Blended");
	}

	if (!this->fastSurfaceDirty) {
		SDL_FillRect(this->fastSurface, nullptr, 0);
	}

	SDL_Rect position = { x, y, 0, 0 }; // w & h are ignored when doing non-scaled blitting
	if (align == AlignH::Center) {
		position.x -= textSurface->w / 2;
	} else if (align == AlignH::Right) {
		position.x -= textSurface->w;
	}
	if (SDL_BlitSurface(textSurface, nullptr, this->fastSurface, &position) != 0) {
		logSDLError("BlitSurface");
	}
	SDL_FreeSurface(textSurface);

	this->fastSurfaceDirty = true;
}

void Hud::drawTextBlended(int x, int y, char *text) {
	drawTextBlended(x, y, text, AlignH::Left);
}

void Hud::drawTextBlended(int x, int y, char *text, AlignH align) {
	SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, this->color);
	if (textSurface == nullptr) {
		logSDLError("TTF_RenderText_Blended");
	}
	//set blend mode to none because we want this surface's alpha to override ontoSurface's alpha (not blend with it)
	if (SDL_SetSurfaceBlendMode(textSurface, SDL_BLENDMODE_NONE) != 0) {
		logSDLError("SetSurfaceBlendMode");
	}

	if (!this->slowSurfaceDirty) {
		SDL_FillRect(this->slowSurface, nullptr, 0);
	}

	SDL_Rect position = { x, y, 0, 0 }; // w & h are ignored when doing non-scaled blitting
	if (align == AlignH::Center) {
		position.x -= textSurface->w / 2;
	} else if (align == AlignH::Right) {
		position.x -= textSurface->w;
	}
	if (SDL_BlitSurface(textSurface, nullptr, this->slowSurface, &position) != 0) {
		logSDLError("BlitSurface");
	}
	SDL_FreeSurface(textSurface);

	this->slowSurfaceDirty = true;
}

void updateTextureFromSurface(SDL_Texture *texture, SDL_Surface *surface) {
	bool requiresLocking = SDL_MUSTLOCK(surface) != 0;

	if (requiresLocking) {
		if (SDL_LockSurface(surface) != 0) {
			logSDLError("SDL_LockSurface()");
		}
	}

	SDL_Rect hudSurfaceRect = { 0, 0, surface->w, surface->h };
	SDL_UpdateTexture(texture, &hudSurfaceRect, surface->pixels, surface->pitch);

	if (requiresLocking) {
		SDL_UnlockSurface(surface);
	}
}

void Hud::render() {
	if (this->slowSurfaceDirty) {
		updateTextureFromSurface(this->slowTexture, this->slowSurface);
		this->slowSurfaceDirty = false;
	}
	if (fastSurfaceDirty) {
		updateTextureFromSurface(this->fastTexture, this->fastSurface);
		this->fastSurfaceDirty = false;
	}

	renderTexture(this->slowTexture, this->renderer, 0, 0);
	renderTexture(this->fastTexture, this->renderer, 0, 0);
}

Hud::~Hud() {
	SDL_FreeSurface(this->fastSurface);
	SDL_DestroyTexture(this->fastTexture);
	SDL_FreeSurface(this->slowSurface);
	SDL_DestroyTexture(this->slowTexture);
	TTF_CloseFont(this->font);
}