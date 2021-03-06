#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

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

bool rects_overlap(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

/**
* Returns an integer between the closed range min to max. (i.e. if min is 1 and max is 3, 
* then 1, 2, or 3 will be returned.)
*/
int randomIntInRange(int min, int max);

/* Has a 50/50 chance of returning 1 or -1 */ 
int randomSignForInt();

class FpsTracker {
public:
	explicit FpsTracker(int numberOfSamples);
	~FpsTracker();
	/**
	* @param frameTime the current frame count
	* @return the running average of the provided frame times
	*/
	float calculateAverageFrameTime(float frameTime);

private:
	DISALLOW_COPY_AND_ASSIGN(FpsTracker);
	int frameIndex;
	float totalFrameTime;
	float *frameTimes;
	int numberOfSamples;
	int samplesSoFar;
};

#endif
