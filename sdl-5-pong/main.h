#ifndef MAIN_H
#define MAIN_H

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

const int PLAYER_SPEED = 5;
const int INITIAL_BALL_X_SPEED = 5;
const int INITIAL_BALL_Y_SPEED_MIN = 2;
const int INITIAL_BALL_Y_SPEED_MAX = 5;

typedef struct Score_struct {
	int human;
	int opponent;
} Score;

#endif
