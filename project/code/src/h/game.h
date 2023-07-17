#ifndef GAME_H_
#define GAME_H_

#include"graphics.h"

enum wsEntityType
	{ kEnvironmentStatic, kEnvironmentDynamic, kLivingStatic, kLivingDynamic };

typedef struct wsEntity
{
	uint8_t entityType;
	uint8_t maxHealth;
	uint8_t health;
	
	// MTG be like: 
	uint16_t power;		// Temporary damage modifier.
	uint16_t defense;	// Temporary defense modifier.
	
	// DnD be like: 
	int8_t strMod;	// Flat damage modifier.
	int8_t dexMod;	// Attack speed modifier.
	int8_t conMod;	// Health point modifier.
	int8_t wisMod;	// Dialogue outcome modifier.
	int8_t intMod;	// Objective speed modifier.
	int8_t stlMod;	// Stealth/footstep volume modifier.
	int8_t spdMod;	// Flat speed modifier.
	int8_t jmpMod;	// Flat jump modifier.
}
wsEntity;

typedef struct wsPlayer
{
	float moveSpeed;
	vec3 position;
	vec3 rotation;
	wsCamera camera;
	wsEntity stats;
}
wsPlayer;

typedef struct wsGame
{
	wsPlayer player;
}
wsGame;

void wsGameInit(wsGame* gameData, wsVulkan* gfx);
void wsGameUpdate(double deltaTime);
void wsGameUpdatePlayerMovement(double deltaTime);
void wsGameSetCameraPointers(wsVulkan* gfx);

#endif