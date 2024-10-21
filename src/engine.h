#pragma once

#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <mutil/mutil.h>
#include <lysys/lysys.hpp>

#define NOISE_TEXTURE_UNIT 27

using namespace mutil;

class Mesh;
class RenderableMesh;
class Shader;
class Skybox;
class Camera;
class Compositor;
class Gbuffer;
class Terrain;
class Bloom;
class Generator;

enum ShaderID
{
    SHADER_COMPOSITE1,
    SHADER_COMPOSITE2,
    SHADER_COMPOSITE3,
    SHADER_COMPOSITE4,
    SHADER_FINAL,

    SHADER_GENERIC,
    SHADER_SKYBOX,
    SHADER_SKYDOME,
    SHADER_TERRAIN,
    SHADER_WATER,

    SHADER_VISUALIZE,

    SHADER_DOWNSAMPLE,
    SHADER_UPSAMPLE,

    SHADER_COUNT
};

enum CompositorID
{
    COMPOSITOR1,
    COMPOSITOR2,
    COMPOSITOR3,
    COMPOSITOR4,

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
    VISUALIZE_MATERIAL,
    VISUALIZE_COMPOSITOR
};

enum TonemapMode
{
    TONEMAP_LINEAR,
    TONEMAP_REINHARD,
    TONEMAP_ACES,
    TONEMAP_UNCHARTED2_RGB,
    TONEMAP_UNCHARTED2_LUM,
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
const IntVector2 &getMouseDelta();
const IntVector2 &getScroll();

void drawQuad();

bool beginFrame();
void renderAll();
void endFrame();

Shader *getShader(ShaderID id);
Gbuffer *getGbuffer();
Compositor *getCompositor(CompositorID id);
Bloom *getBloom();

Camera *getCamera();

const std::vector<RenderableMesh *> &getMeshes();
void addMesh(RenderableMesh *mesh);

const std::vector<Terrain *> &getTerrains();
void addTerrain(Terrain *terrain);

Terrain *getWater();

Skybox *getSkybox();

Generator *getTerrainGenerator();

Mesh *getCubeMesh();

VisualizeMode getVisualizeMode();
void setVisualizeMode(VisualizeMode mode);
void setVisualizeCompositor(CompositorID id);

bool getWireframe();
void setWireframe(bool enabled);

float getExposure();
void setExposure(float exposure);

float getGamma();
void setGamma(float gamma);

float getBloomStrength();
void setBloomStrength(float strength);

bool getVsync();
void setVsync(bool enabled);

bool getFXAAEnabled();
void setFXAAEnabled(bool enabled);

TonemapMode getTonemapMode();
void setTonemapMode(TonemapMode mode);

GLuint getNoise();
