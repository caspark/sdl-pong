#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

void handleFatal(int exitcode);

void logSDLError(const std::string &sdlFunctionName);

void logFatal(const std::string &msg);

/**
* Loads an image into a texture on the rendering device
* @param file The image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to (fractional component will be discarded)
* @param y The y coordinate to draw to (fractional component will be discarded)
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, float x, float y);

/**
* @param color - the color to render the text (alpha component is ignored and always set to 255)
*/
void renderText(char *text, TTF_Font *font, SDL_Color color, SDL_Surface *ontoSurface, int x, int y);

#endif
