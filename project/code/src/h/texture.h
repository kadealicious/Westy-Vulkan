#ifndef TEXTURE_H_
#define TEXTURE_H_


#include<vulkan/vulkan.h>

#define WS_MAX_TEXTURES 100

typedef struct wsTexture
{
    VkImage			image;
    VkDeviceMemory	memory;
	VkImageView		view;
}
wsTexture;

typedef struct wsTextureManager
{
    uint8_t num_active_meshes;
    uint8_t isloaded[WS_MAX_TEXTURES];
    wsTexture texture[WS_MAX_TEXTURES];
}
wsTexMan;


void wsTextureInit(wsTexMan* textureManager);
void wsTextureTerminate();
uint8_t wsTextureCreate(const char* texPath, uint16_t texWidth, uint16_t texHeight);
void wsTextureUnload(uint8_t* texIDs, uint8_t numTextures);


#endif