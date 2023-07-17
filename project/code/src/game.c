#include<stdlib.h>

#include"h/game.h"
#include"h/camera.h"
#include"h/input.h"

wsGame* game;

void wsGameUpdatePlayerMovement();

void wsGameInit(wsGame* gameData, wsVulkan* gfx)
{
	game = gameData;
	
	// TODO: This should probably not be handled here.
	// Set camera pointers so we can render from our main camera.
	gfx->cameraPosition		= &game->player.camera.position;
	gfx->cameraForward		= &game->player.camera.forward;
	gfx->cameraUp			= &game->player.camera.up;
	gfx->cameraProjection	= &game->player.camera.projection;
	
	glm_vec3_copy((vec3){0.0f, 0.0f, 2.0f}, game->player.position);
	game->player.moveSpeed = 2.0f;
	
	wsEntity* playerStats = &game->player.stats;
	playerStats->entityType = kLivingDynamic;
	
	bool statsAreBad = true;
	while(statsAreBad)
	{
		playerStats->strMod = (rand() % 20) - 10;
		playerStats->dexMod = (rand() % 20) - 10;
		playerStats->conMod = (rand() % 20) - 10;
		playerStats->wisMod = (rand() % 20) - 10;
		playerStats->intMod = (rand() % 20) - 10;
		playerStats->stlMod = (rand() % 20) - 10;
		playerStats->spdMod = (rand() % 20) - 10;
		playerStats->jmpMod = (rand() % 20) - 10;
		int8_t statsTotal = playerStats->strMod + playerStats->dexMod + playerStats->conMod + 
			playerStats->wisMod + playerStats->intMod + playerStats->stlMod + playerStats->spdMod + 
			playerStats->jmpMod;
		
		/* printf("Player has been assigned stats w/ magnitude %i: %i %i %i %i %i %i\n", 
			statsTotal, playerStats->strMod, playerStats->dexMod, playerStats->conMod, 
			playerStats->wisMod, playerStats->intMod, playerStats->stlMod); */
		
		if(statsTotal > 8)
			statsAreBad = false;
	}
	
	playerStats->maxHealth	= 10 + (playerStats->conMod / 2);
	playerStats->health		= playerStats->maxHealth;
}

void wsGameUpdate(double deltaTime)
{
	wsCameraUpdateProjection(&game->player.camera);
}

void wsGameUpdatePlayerMovement(double deltaTime)
{
	wsCameraUpdateFPSLook(&game->player.camera, &game->player.position, deltaTime);
	
	float moveSpeed = game->player.moveSpeed + (game->player.stats.spdMod * 0.15f);
	float keys_vertical = -(wsInputGetKeyHold(GLFW_KEY_SPACE) - wsInputGetKeyHold(GLFW_KEY_LEFT_CONTROL)) * deltaTime * moveSpeed;
	float keys_horizontal = -(wsInputGetKeyHold(GLFW_KEY_D) - wsInputGetKeyHold(GLFW_KEY_A)) * deltaTime * moveSpeed;
	float keys_forward = (wsInputGetKeyHold(GLFW_KEY_W) - wsInputGetKeyHold(GLFW_KEY_S)) * deltaTime * moveSpeed;
	
	vec3 move_vertical;
	vec3 move_horizontal;
	vec3 move_forward;
	vec3 worldUp = {0.0f, 1.0f, 0.0f};
	
	glm_vec3_scale(worldUp, keys_vertical, move_vertical);
	glm_vec3_scale(game->player.camera.right, keys_horizontal, move_horizontal);
	
	vec3 forward_xz_plane = {game->player.camera.forward[0], 0.0f, game->player.camera.forward[2]};
	glm_vec3_normalize(forward_xz_plane);
	glm_vec3_scale(forward_xz_plane, keys_forward, move_forward);
	
	// Move on axes.
	glm_vec3_add(move_vertical, game->player.position, game->player.position);
	glm_vec3_add(move_horizontal, game->player.position, game->player.position);
	glm_vec3_add(move_forward, game->player.position, game->player.position);
}