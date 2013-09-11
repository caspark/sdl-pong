const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

const int PLAYER_SPEED = 5;
const int INITIAL_BALL_X_SPEED = 5;
const int INITIAL_BALL_Y_SPEED_MIN = 2;
const int INITIAL_BALL_Y_SPEED_MAX = 5;

typedef struct Vector2_struct {
	float x;
	float y;
} Vector2;

class Player {
public:
	Vector2 pos;
	Vector2 size;

	Player(SDL_Renderer *renderer, bool isLeft);
	~Player();

	void render();

	Vector2 getCenter();

private:
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

class Ball {
public:
	Vector2 pos;
	Vector2 speed;
	Vector2 size;

	Ball(SDL_Renderer *renderer);
	~Ball();

	void render();

	Vector2 getCenter();

private:
	SDL_Renderer *renderer;
	SDL_Texture *tex;
};

typedef struct Score_struct {
	int human;
	int opponent;
} Score;

class Hud {
public:
	Hud(SDL_Renderer *renderer);
	~Hud();

	void update();
	void render();

private:
	SDL_Renderer *renderer;
	TTF_Font *font;
	SDL_Surface *surface;
	SDL_Texture *texture;
};