#include<stdio.h>
#include<stdbool.h>
#include"h/texture.h"
#include"h/graphics.h"


wsTexMan* texMan;


void wsTextureInit(wsTexMan* textureManager)
{
    texMan = textureManager;
    texMan->num_active_meshes = 0;
    uint8_t defaultTex = wsTextureCreate("textures/bobross.png", 800, 600);

    printf("INFO: Texture manager initialized!\n");
}

void wsTextureTerminate()
{
    printf("INFO: Texture manager terminated!\n");
}

uint8_t wsTextureCreate(const char* texPath, uint16_t texWidth, uint16_t texHeight)
{
    static uint8_t texID = 0;

    // TODO: i dont even know dude
    wsVulkanCreateTexture(texPath);

    texMan->num_active_meshes++;
    texMan->isloaded[texID] = true;

    texID++;
    return (texID - 1);
}

void wsTextureUnload(uint8_t* texIDs, uint8_t numTextures)
{
    if(texIDs == NULL)
    {
        numTextures = texMan->num_active_meshes;
    }

    for(uint8_t i = 0; i < numTextures; i++)
    {
        if(texMan->isloaded[i])
        {
            wsVulkanDestroyTexture(i);
            texMan->isloaded[i] = false;
        }
    }

    texMan->num_active_meshes -= numTextures;
    printf("INFO: Texture manager destroyed %i textures!\n", numTextures);
}