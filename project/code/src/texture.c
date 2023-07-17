#include"h/texture.h"
#include"h/graphics.h"

typedef struct wsTextureManager
{
	wsTexture fallbackTexture;
	VkDevice* logicalDevice;
}
wsTexMan;
wsTexMan texMan;

wsTexture* wsTextureInit(VkDevice* logicalDevice)
{
	texMan.logicalDevice = logicalDevice;
	wsTextureCreate("textures/bobross.png", &texMan.fallbackTexture);
	return &texMan.fallbackTexture;
}

void wsTextureCreate(const char* texPath, wsTexture* texture)
{
	if(wsVulkanCreateTextureImage(&texture->image, &texture->memory, texPath) != VK_SUCCESS)
		{ texture = &texMan.fallbackTexture; return; }
	if(wsVulkanCreateImageView(&texture->image, &texture->view, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT) != VK_SUCCESS)
		{ texture = &texMan.fallbackTexture; return; }	
}

void wsTextureDestroy(wsTexture* texture)
{
	vkDestroyImageView(*texMan.logicalDevice, texture->view, NULL);
	vkDestroyImage(*texMan.logicalDevice, texture->image, NULL);
	vkFreeMemory(*texMan.logicalDevice, texture->memory, NULL);
}