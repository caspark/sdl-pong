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

typedef struct Player_struct {
	Vector2 pos;
	Vector2 size;
	SDL_Texture *tex;
} Player;

typedef struct Ball_struct {
	Vector2 pos;
	Vector2 speed;
	Vector2 size;
	SDL_Texture *tex;
} Ball;

typedef struct Score_struct {
	int human;
	int opponent;
} Score;

typedef struct Hud_struct {
	TTF_Font *font;
	SDL_Surface *surface;
	SDL_Texture *texture;
} Hud;