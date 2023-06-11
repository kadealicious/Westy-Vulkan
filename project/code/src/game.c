#include"h/game.h"
#include"h/camera.h"

wsGame* game;

void wsGameInit(wsGame* gameData)
{
	game = gameData;
	
	// Initialize cameras!
	wsCameraInit(&game->camera);
	game->mainCameraID = wsCameraCreate();
}

void wsGameUpdate(double deltaTime)
{
	wsCameraUpdateProjection(game->mainCameraID);
}

void wsGameUpdateFPSCamera(double deltaTime)
{
	wsCameraUpdateFPSCamera(game->mainCameraID, deltaTime);
}

void wsGameSetCameraPointers(wsVulkan* gfx)
{
	gfx->cameraPosition		= wsCameraGetPositionPtr(game->mainCameraID);
	gfx->cameraForward		= wsCameraGetForwardPtr(game->mainCameraID);
	gfx->cameraUp			= wsCameraGetUpPtr(game->mainCameraID);
	gfx->cameraProjection	= wsCameraGetProjectionPtr(game->mainCameraID);
}