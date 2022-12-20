#include<stdio.h>
#include<stdlib.h>

#include"h/shader.h"

void wsShaderInit(wsShader* shd);
uint8_t wsShaderLoad(wsShader* shd, const char* path);
void wsShaderUnloadAll(wsShader* shd);
void wsShaderUnload(wsShader* shd, uint8_t shaderID);

void wsShaderInit(wsShader* shd) {
    for(uint8_t i = 0; i < NUM_MAX_SHADERS; i++) {
        shd->isloaded[i] = false;
    }

    printf("Shaders initialized!\n");
}

uint8_t wsShaderLoad(wsShader* shd, const char* path) {
    enum ERROR_CODES {FILE_NOT_FOUND = (NUM_MAX_SHADERS + 1)};
    static uint8_t current_shaderID = 0;   // Used for shader identification within struct wsShader.

    // Open shader file; if not found, return NULL.
    FILE* file = fopen(path, "rb");
    if(file == NULL) {
        printf("ERROR: Shader \"%s\" not found!\n", path);
        return FILE_NOT_FOUND;
    } else printf("Shader \"%s\" found!\n", path);

    // Get file size.
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    
    // Read binary data into file.
    shd->shader_data[current_shaderID] = malloc(file_size * sizeof(char));
    uint32_t size_read = fread(shd->shader_data[current_shaderID], sizeof(char), file_size, file);
    printf("%i/%i bytes read!\n", size_read, file_size);

    // Close file.
    fclose(file);

    printf("Shader ID %i loaded!\n", current_shaderID);
    return current_shaderID++;
}

void wsShaderUnloadAll(wsShader* shd) {
    for(uint8_t i = 0; i < NUM_MAX_SHADERS; i++) {
        if(shd->isloaded[i]) {
            free(shd->shader_data[i]);
            shd->isloaded[i] = false;
        }
    }
}

void wsShaderUnload(wsShader* shd, uint8_t shaderID) {
    if(shd->isloaded[shaderID]) {
        free(shd->shader_data[shaderID]);
        shd->isloaded[shaderID] = false;
    }
}