#include <string>
#include <iostream>

#include "util.h"

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

void renderText(char *text, TTF_Font *font, SDL_Color color, SDL_Surface *ontoSurface, int x, int y) {
	SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, color);
	if (textSurface == nullptr) {
		logSDLError("TTF_RenderText_Blended");
	}
	//set blend mode to none because we want this surface's alpha to override ontoSurface's alpha (not blend with it)
	if (SDL_SetSurfaceBlendMode(textSurface, SDL_BLENDMODE_NONE) != 0) {
		logSDLError("SetSurfaceBlendMode");
	}

	SDL_Rect position = { x, y, 0, 0 }; // w & h are ignored when doing non-scaled blitting
	if (SDL_BlitSurface(textSurface, nullptr, ontoSurface, &position) != 0) {
		logSDLError("BlitSurface");
	}
	SDL_FreeSurface(textSurface);
}