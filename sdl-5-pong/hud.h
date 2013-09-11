#ifndef HUD_H
#define HUD_H

class Hud {
public:
	Hud(SDL_Renderer *renderer, int screenWidth, int screenHeight);
	~Hud();

	void update();
	void render();

private:
	SDL_Renderer *renderer;
	TTF_Font *font;
	SDL_Surface *surface;
	SDL_Texture *texture;
};

#endif