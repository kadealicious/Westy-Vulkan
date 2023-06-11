#ifndef GAME_H_
#define GAME_H_

#include"graphics.h"

typedef struct wsGame
{
	wsCamera camera;
	uint8_t mainCameraID;
}
wsGame;

void wsGameInit(wsGame* gameData);
void wsGameUpdate(double deltaTime);
void wsGameUpdateFPSCamera(double deltaTime);
void wsGameSetCameraPointers(wsVulkan* gfx);

#endif