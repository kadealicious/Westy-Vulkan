#ifndef TEXTURE_H_
#define TEXTURE_H_


#include<vulkan/vulkan.h>

#define WS_TEXTURE_MAX_TEXTURES 100

typedef struct wsTexture
{
	uint32_t mipLevels;
    VkImage			image;
    VkDeviceMemory	memory;
	VkImageView		view;
}
wsTexture;

wsTexture* wsTextureInit(VkDevice* logicalDevice);	// Returns a pointer to the fallback texture.
void wsTextureCreateFallback(const char* texPath, wsTexture* fallbackTexture);
void wsTextureCreate(const char* texPath, wsTexture* texture);
void wsTextureDestroy(wsTexture* texture);


#endif