#pragma once

#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <mutil/mutil.h>
#include <lysys/lysys.hpp>

using namespace mutil;

class Mesh;
class Shader;
class Skybox;
class Camera;

enum ShaderID
{
    SHADER_COMPOSITE1,
    SHADER_COMPOSITE2,

    SHADER_GENERIC,
    SHADER_SKYBOX,

    SHADER_COUNT
};

void initAll(int argc, char *argv[]);
void quitAll();

LS_NORETURN void fatal(const char *format, ...);

SDL_Window *getWindow();
const IntVector2 &getWindowSize();

float getTime();
float deltaTime();

bool getKey(int key);
bool getMouseButton(int button);
const IntVector2 &getMousePos();

void drawQuad();

bool beginFrame();
void renderAll();
void endFrame();

Shader *getShader(ShaderID id);

Camera *getCamera();

const std::vector<Mesh *> &getMeshes();
void addMesh(Mesh *mesh);

Skybox *getSkybox();
