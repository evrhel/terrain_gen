#pragma once

#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <mutil/mutil.h>
#include <lysys/lysys.hpp>

using namespace mutil;

class Mesh;
class RenderableMesh;
class Shader;
class Skybox;
class Camera;
class Compositor;
class Gbuffer;
class Terrain;

enum ShaderID
{
    SHADER_COMPOSITE1,
    SHADER_COMPOSITE2,

    SHADER_GENERIC,
    SHADER_SKYBOX,
    SHADER_TERRAIN,

    SHADER_VISUALIZE,

    SHADER_DOWNSAMPLE,
    SHADER_UPSAMPLE,

    SHADER_COUNT
};

enum CompositorID
{
    COMPOSITOR1,
    COMPOSITOR2,

    COMPOSITOR_COUNT
};

enum VisualizeMode
{
    VISUALIZE_NONE,
    VISUALIZE_ALBEDO,
    VISUALIZE_EMISSIVE,
    VISUALIZE_POSITION,
    VISUALIZE_DEPTH,
    VISUALIZE_NORMAL,
    VISUALIZE_MATERIAL
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
Gbuffer *getGbuffer();
Compositor *getCompositor(CompositorID id);

Camera *getCamera();

const std::vector<RenderableMesh *> &getMeshes();
void addMesh(RenderableMesh *mesh);

const std::vector<Terrain *> &getTerrains();
void addTerrain(Terrain *terrain);

Skybox *getSkybox();

Mesh *getCubeMesh();

VisualizeMode getVisualizeMode();
void setVisualizeMode(VisualizeMode mode);

bool getWireframe();
void setWireframe(bool enabled);
