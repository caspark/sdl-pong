#ifndef HUD_H
#define HUD_H

class Hud {
public:
	Hud(SDL_Renderer *renderer, int screenWidth, int screenHeight);
	~Hud();

	void setTextColor(uint8_t r, uint8_t g, uint8_t b);
	void drawTextFast(int x, int y, const char *text);
	void drawTextBlended(int x, int y, char *text);

	void render();

private:
	SDL_Renderer *renderer;
	SDL_Color color;
	TTF_Font *font;
	SDL_Surface *slowSurface;
	bool slowSurfaceDirty;
	SDL_Texture *slowTexture;
	SDL_Surface *fastSurface;
	bool fastSurfaceDirty;
	SDL_Texture *fastTexture;
};

#endif