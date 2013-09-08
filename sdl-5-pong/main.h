const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

const int PLAYER_SPEED = 5;
const int INITIAL_BALL_X_SPEED = 5;
const int INITIAL_BALL_Y_SPEED_MIN = 2;
const int INITIAL_BALL_Y_SPEED_MAX = 5;

typedef struct vec2_struct {
	int x;
	int y;
} VEC2;

typedef struct player_struct {
	VEC2 pos;
	VEC2 size;
	SDL_Texture *tex;
} PLAYER;

typedef struct ball_struct {
	VEC2 pos;
	VEC2 speed;
	VEC2 size;
	SDL_Texture *tex;
} BALL;

typedef struct score_struct {
	int human;
	int opponent;
} SCORE;

typedef struct hud_struct {
	TTF_Font *font;
	SDL_Surface *surface;
	SDL_Texture *texture;
} Hud;