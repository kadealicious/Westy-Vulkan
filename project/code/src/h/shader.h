#ifndef LOAD_SHADER_H_
#define LOAD_SHADER_H_

#include<stdint.h>
#include<stdbool.h>

#define NUM_MAX_SHADERS 100

typedef struct wsShader {
    bool isloaded[NUM_MAX_SHADERS];             // Says whether or not shader is loaded into memory.
    char* shader_data[NUM_MAX_SHADERS];         // Holds bytecode shader data for all shaders.

} wsShader;

void wsShaderInit(wsShader* shd);
uint8_t wsShaderLoad(wsShader* shd, const char* path);
void wsShaderUnloadAll(wsShader* shd);
void wsShaderUnload(wsShader* shd, uint8_t shaderID);

#endif