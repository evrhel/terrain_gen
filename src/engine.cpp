#include "engine.h"

#include <cstdarg>
#include <cstdio>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include <lysys/lysys.hpp>

#include <half.hpp>

#include "mesh.h"
#include "shader.h"
#include "skybox.h"
#include "camera.h"
#include "util.h"
#include "composite.h"
#include "gbuffer.h"
#include "terrain.h"
#include "bloom.h"
#include "generator.h"

static const Vector4 kQuadVertices[] = {
    Vector4(-1.0f, -1.0f, 0.0f, 0.0f),
    Vector4(1.0f, -1.0f, 1.0f, 0.0f),
    Vector4(1.0f, 1.0f, 1.0f, 1.0f),
    Vector4(-1.0f, 1.0f, 0.0f, 1.0f)};

static const GLuint kQuadIndices[] = {
    0, 1, 2,
    2, 3, 0};

static const Vertex kCubeVertices[] = {
    /* Front */
    Vertex{
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(0.0f, 0.0f, 1.0f),
    },
    Vertex{
        Vector3(0.5f, -0.5f, 0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(0.0f, 0.0f, 1.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, 0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f),
    },
    Vertex{
        Vector3(-0.5f, 0.5f, 0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f),
    },
    /* Back */
    Vertex{
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(0.0f, 0.0f, -1.0f),
    },
    Vertex{
        Vector3(0.5f, -0.5f, -0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(0.0f, 0.0f, -1.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, -0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
    },
    Vertex{
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(0.0f, 0.0f, -1.0f),
    },
    /* Left */
    Vertex{
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(-1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(-1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(-0.5f, 0.5f, 0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(-1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(-1.0f, 0.0f, 0.0f),
    },
    /* Right */
    Vertex{
        Vector3(0.5f, -0.5f, -0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, -0.5f, 0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, 0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(1.0f, 0.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, -0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(1.0f, 0.0f, 0.0f),
    },
    /* Top */
    Vertex{
        Vector3(-0.5f, 0.5f, -0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, -0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, 0.5f, 0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(0.0f, 1.0f, 0.0f),
    },
    Vertex{
        Vector3(-0.5f, 0.5f, 0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(0.0f, 1.0f, 0.0f),
    },
    /* Bottom */
    Vertex{
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector2(0.0f, 0.0f),
        Vector3(0.0f, -1.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, -0.5f, -0.5f),
        Vector2(1.0f, 0.0f),
        Vector3(0.0f, -1.0f, 0.0f),
    },
    Vertex{
        Vector3(0.5f, -0.5f, 0.5f),
        Vector2(1.0f, 1.0f),
        Vector3(0.0f, -1.0f, 0.0f),
    },
    Vertex{
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector2(0.0f, 1.0f),
        Vector3(0.0f, -1.0f, 0.0f),
    }};

static const GLuint kCubeIndices[] = {
    /* Front */
    0, 1, 2,
    2, 3, 0,

    /* Back */
    6, 5, 4,
    4, 7, 6,

    /* Left */
    8, 9, 10,
    10, 11, 8,

    /* Right */
    14, 13, 12,
    12, 15, 14,

    /* Top */
    18, 17, 16,
    16, 19, 18,

    /* Bottom */
    20, 21, 22,
    22, 23, 20};

#define CUBE_VERTEX_COUNT 24
#define CUBE_INDEX_COUNT 36

static SDL_Window *_window;
static SDL_GLContext _gl;

static GLuint _quadVao, _quadVbo, _quadEbo;

static constexpr int kNoiseTexSize = 128;
static GLuint _noiseTex;

static IntVector2 _windowSize;

static double _startTime;

static float _startFrameTime;
static float _deltaTime;
static int _frame;

static bool _keys[SDL_NUM_SCANCODES];
static bool _mouseButtons[8];
static IntVector2 _mousePosition;
static IntVector2 _mouseDelta;
static IntVector2 _scroll;

static Shader *_shaders[SHADER_COUNT];
static Gbuffer *_gbuffer;
static Compositor *_compositors[COMPOSITOR_COUNT];
static Compositor *_final;
static Compositor *_visualizer;
static Bloom *_bloom;

static Camera *_camera;

static std::vector<RenderableMesh *> _meshes;
static std::vector<Terrain *> _terrains;
static Terrain *_water;
static Skybox *_skybox;
static Generator *_generator;

static Mesh *_cube;

static VisualizeMode _visualizeMode = VISUALIZE_NONE;
static CompositorID _visualizeCompositor = COMPOSITOR1;
static bool _wireframe = false;

static float _exposure = 1.0f;
static float _gamma = 2.2f;
static float _bloomStrength = 1.0f;

static bool _vsync = true;

static bool _fxaa = true;

#define SIZE_EV 64

static bool pollEvents()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        ImGui_ImplSDL3_ProcessEvent(&evt);

        switch (evt.type)
        {
        default:
            break;
        case SDL_EVENT_QUIT:
            return false; // Quit the main loop
        case SDL_EVENT_WINDOW_RESIZED:
            _windowSize.x = evt.window.data1;
            _windowSize.y = evt.window.data2;
            
            _gbuffer->resize(_windowSize.x, _windowSize.y);
            _bloom->resize(_windowSize.x, _windowSize.y);

            for (int i = 0; i < COMPOSITOR_COUNT; i++)
				_compositors[i]->resize(_windowSize.x, _windowSize.y);
            _final->resize(_windowSize.x, _windowSize.y);
            _visualizer->resize(_windowSize.x, _windowSize.y);

            break;
        case SDL_EVENT_KEY_DOWN:
            _keys[(int)evt.key.scancode] = true;
            break;
        case SDL_EVENT_KEY_UP:
            _keys[(int)evt.key.scancode] = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            _mouseButtons[evt.button.button] = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            _mouseButtons[evt.button.button] = false;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            _mousePosition.x = (int32_t)evt.motion.x;
            _mousePosition.y = (int32_t)evt.motion.y;
            _mouseDelta.x += (int32_t)evt.motion.xrel;
            _mouseDelta.y += (int32_t)evt.motion.yrel;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            _scroll.x += (int32_t)evt.wheel.x;
            _scroll.y += (int32_t)evt.wheel.y;
            break;
        }
    }

    return true;
}

static void createQuad()
{
    glGenVertexArrays(1, &_quadVao);

    glBindVertexArray(_quadVao);

    glGenBuffers(1, &_quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &_quadEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), (void *)0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkGLErrors("createQuad");
}

static void destroyQuad()
{
    glDeleteBuffers(1, &_quadEbo);
    glDeleteBuffers(1, &_quadVbo);
    glDeleteVertexArrays(1, &_quadVao);
}

static void createNoiseTex()
{
    constexpr int kCount = kNoiseTexSize * kNoiseTexSize;
    half_float::half *noiseTex = new half_float::half[kCount];

    for (int i = 0; i < kCount; i++)
        noiseTex[i] = ls_rand_float();

    glGenTextures(1, &_noiseTex);

    glBindTexture(GL_TEXTURE_2D, _noiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, kNoiseTexSize, kNoiseTexSize, 0, GL_RED, GL_HALF_FLOAT, noiseTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] noiseTex;

    checkGLErrors("createNoiseTex");
}

static void destroyNoiseTex()
{
    glDeleteTextures(1, &_noiseTex);
}

/* Sources */
namespace
{
#include <shaders/composite1.frag.h>
#include <shaders/composite2.frag.h>
#include <shaders/composite3.frag.h>
#include <shaders/downsample.frag.h>
#include <shaders/final.frag.h>
#include <shaders/generic.frag.h>
#include <shaders/generic.vert.h>
#include <shaders/screen.vert.h>
#include <shaders/skybox.frag.h>
#include <shaders/skybox.vert.h>
#include <shaders/skydome.vert.h>
#include <shaders/skydome.geom.h>
#include <shaders/skydome.frag.h>
#include <shaders/terrain.frag.h>
#include <shaders/terrain.tcs.h>
#include <shaders/terrain.tes.h>
#include <shaders/terrain.vert.h>
#include <shaders/upsample.frag.h>
#include <shaders/visualize.frag.h>
#include <shaders/water.frag.h>
#include <shaders/water.tes.h>
}

static void loadShaders()
{
    for (int32_t i = 0; i < SHADER_COUNT; i++)
        _shaders[i] = new Shader();

    /* Load shaders */

    Shader *composite1 = getShader(SHADER_COMPOSITE1);
    composite1->load("composite1", screen_vert_source, composite1_frag_source);

    Shader *composite2 = getShader(SHADER_COMPOSITE2);
    composite2->load("composite2", screen_vert_source, composite2_frag_source);

    Shader *composite3 = getShader(SHADER_COMPOSITE3);
    composite3->load("composite3", screen_vert_source, composite3_frag_source);

    Shader *final = getShader(SHADER_FINAL);
    final->load("final", screen_vert_source, final_frag_source);

    Shader *generic = getShader(SHADER_GENERIC);
    generic->load("generic", generic_vert_source, generic_frag_source);

    Shader *skybox = getShader(SHADER_SKYBOX);
    skybox->load("skybox", skybox_vert_source, skybox_frag_source);

    Shader *skydome = getShader(SHADER_SKYDOME);
    skydome->loadGeom("skydome", skydome_vert_source, skydome_geom_source, skydome_frag_source);

    Shader *terrain = getShader(SHADER_TERRAIN);
    terrain->loadTess("terrain", terrain_vert_source, terrain_frag_source, terrain_tcs_source, terrain_tes_source);

    Shader *visualize = getShader(SHADER_VISUALIZE);
    visualize->load("visualize", screen_vert_source, visualize_frag_source);

    Shader *downsample = getShader(SHADER_DOWNSAMPLE);
    downsample->load("downsample", screen_vert_source, downsample_frag_source);

    Shader *upsample = getShader(SHADER_UPSAMPLE);
    upsample->load("upsample", screen_vert_source, upsample_frag_source);

    Shader *water = getShader(SHADER_WATER);
	water->loadTess("water", terrain_vert_source, water_frag_source, terrain_tcs_source, water_tes_source);
}

static void destroyShaders()
{
    for (int i = 0; i < SHADER_COUNT; i++)
        delete _shaders[i];
}

static void loadCompositors(GLsizei width, GLsizei height)
{
    OutputSpec outputs[MAX_COMPOSITOR_OUTPUTS];
    OutputSpec &texture0 = outputs[0];
    OutputSpec &texture1 = outputs[1];
    OutputSpec &texture2 = outputs[2];
    OutputSpec &texture3 = outputs[3];

    /* Compositor 1 */
    Compositor *compositor1 = new Compositor();
    texture0.internalFormat = GL_R11F_G11F_B10F;
    texture0.format = GL_RGB;
    texture0.type = GL_FLOAT;
    compositor1->load(outputs, 1);
    _compositors[COMPOSITOR1] = compositor1;

    /* Compositor 2 */
    Compositor *compositor2 = new Compositor();
    texture0.internalFormat = GL_R11F_G11F_B10F;
    texture0.format = GL_RGB;
    texture0.type = GL_FLOAT;
    compositor2->load(outputs, 1);
    _compositors[COMPOSITOR2] = compositor2;

    /* Compositor 3 */
    Compositor *compositor3 = new Compositor();
    texture0.internalFormat = GL_R11F_G11F_B10F;
    texture0.format = GL_RGB;
    texture0.type = GL_FLOAT;
    compositor3->load(outputs, 1);
    _compositors[COMPOSITOR3] = compositor3;

    /* Final */
    _final = new Compositor();
    _final->load(nullptr, 0); // No outputs, go to screen

    /* Visualizer */
    _visualizer = new Compositor();
    _visualizer->load(nullptr, 0); // No outputs, go to screen

    /* Initial resize */
    for (int i = 0; i < COMPOSITOR_COUNT; i++)
        _compositors[i]->resize(width, height);
    _final->resize(width, height);
    _visualizer->resize(width, height);
}

static void destroyCompositors()
{
    delete _visualizer;
    delete _final;

    for (int i = 0; i < COMPOSITOR_COUNT; i++)
        delete _compositors[i];
}

void initAll(int argc, char *argv[])
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;

    /* Initialize SDL */

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        fatal("SDL_Init: %s\n", SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    /* Create a window */
    _window = SDL_CreateWindow(
        "Terrain Generator",
        kWindowWidth, kWindowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window)
        fatal("SDL_CreateWindow: %s\n", SDL_GetError());

    _windowSize = IntVector2{kWindowWidth, kWindowHeight};

    /* Create OpenGL context */

    _gl = SDL_GL_CreateContext(_window);
    if (!_gl)
        fatal("SDL_GL_CreateContext: %s\n", SDL_GetError());

    if (!gladLoadGLLoader((void *(*)(const char *))SDL_GL_GetProcAddress))
        fatal("gladLoadGLLoader failed\n");

    /* Setup ImGui */

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(_window, _gl);
    ImGui_ImplOpenGL3_Init();

    /* Default default primitives */

    createQuad();

    _cube = new Mesh();
    _cube->load(kCubeVertices, CUBE_VERTEX_COUNT, kCubeIndices, CUBE_INDEX_COUNT);

    createNoiseTex();

    /* Enable vsync */
    _vsync = true;
    SDL_GL_SetSwapInterval(_vsync ? 1 : 0);

    printf("OpenGL : %s\n", glGetString(GL_VERSION));
    printf("GLSL   : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor : %s\n", glGetString(GL_VENDOR));

    /* Load shaders */
    loadShaders();

    /* Create G-buffer */
    _gbuffer = new Gbuffer();
    _gbuffer->load();
    _gbuffer->resize(kWindowWidth, kWindowHeight);

    /* Load compositors */
    loadCompositors(kWindowWidth, kWindowHeight);

    /* Create bloom manager */
    _bloom = new Bloom();
    _bloom->load();
    _bloom->resize(kWindowWidth, kWindowHeight);

    /* Setup camera */
    _camera = new Camera();
    _camera->load();

    /* Create skybox */
    _skybox = new Skybox();
    _skybox->load();

    /* Create water */
    _water = new Terrain();
    _water->setUseMaterials(false);
    _water->setEnabled(false);

    /* Create terrain generator */
    _generator = new Generator();

    /* Initialize time */
    _startTime = ls_time64();
    _startFrameTime = _startTime;
    _frame = 0;
}

void quitAll()
{
    /* Destroy in reverse order */

    for (Terrain *terrain : _terrains)
        terrain->release();
    _terrains.clear();

    for (RenderableMesh *mesh : _meshes)
        mesh->release();
    _meshes.clear();

    delete _generator;

    delete _skybox;

    unloadMaterials();
    unloadTextures();

    delete _camera;

    destroyCompositors();

    delete _gbuffer;

    destroyShaders();

    destroyNoiseTex();

    _cube->release();

    destroyQuad();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(_gl);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void fatal(const char *format, ...)
{
    va_list ls;

    va_start(ls, format);
    vfprintf(stderr, format, ls);
    va_end(ls);

    exit(1);
}

SDL_Window *getWindow()
{
    return _window;
}

const IntVector2 &getWindowSize()
{
    return _windowSize;
}

float getTime()
{
    return (float)(ls_time64() - _startTime);
}

float deltaTime()
{
    return _deltaTime;
}

bool getKey(int key)
{
    if (key < 0 || key >= SDL_NUM_SCANCODES)
        return false;
    return _keys[key];
}

bool getMouseButton(int button)
{
    if (button < 0 || button >= 8)
        return false;
    return _mouseButtons[button];
}

const IntVector2 &getMousePos()
{
    return _mousePosition;
}

const IntVector2 &getMouseDelta()
{
    return _mouseDelta;
}

const IntVector2 &getScroll()
{
    return _scroll;
}

void drawQuad()
{
    glBindVertexArray(_quadVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool beginFrame()
{
    float time = getTime();

    _deltaTime = time - _startFrameTime;
    _startFrameTime = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    _mouseDelta = IntVector2{0, 0};
    _scroll = IntVector2{0, 0};
    return pollEvents();
}

void renderAll()
{
    /* Update camera */
    _camera->update();

    /* Re-render skybox if needed */
    if (_skybox->update(_camera))
    {
        _skybox->renderSkybox();
        _skybox->renderIrradiance();
    }

    /* Render to Gbuffer */

    _gbuffer->bind();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    if (_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Render meshes */

    Shader *genericShader = getShader(SHADER_GENERIC);
    genericShader->use();

    genericShader->setBool("uWireframe", _wireframe);
    genericShader->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
    genericShader->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

    glDepthFunc(GL_LESS);
    for (RenderableMesh *mesh : _meshes)
    {
        if (mesh->enabled())
        {
            mesh->update();
            mesh->render(genericShader);
        }
    }

    /* Render terrains */

    Shader *terrainShader = getShader(SHADER_TERRAIN);
    terrainShader->use();

    terrainShader->setBool("uWireframe", _wireframe);
    terrainShader->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
    terrainShader->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

    for (Terrain *terrain : _terrains)
    {
        if (terrain->enabled())
        {
            terrain->update();
            terrain->render(terrainShader);
        }
    }

    /* Render generated terrain */
    _generator->update();
    _generator->render(terrainShader);

    /* Water */
    if (_water->enabled())
    {
        glDisable(GL_CULL_FACE); // Water is double-sided

        Shader *waterShader = getShader(SHADER_WATER);
        waterShader->use();

        waterShader->setBool("uWireframe", _wireframe);
        waterShader->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
        waterShader->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

        _water->render(waterShader);

        glEnable(GL_CULL_FACE);
    }

    if (_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Render skybox */

    Shader *skyboxShader = getShader(SHADER_SKYBOX);
    skyboxShader->use();

    skyboxShader->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
    skyboxShader->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

    glDepthFunc(GL_LEQUAL);
    _skybox->render(skyboxShader);

    /* Composite render */

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (_visualizeMode == VISUALIZE_NONE || _visualizeMode == VISUALIZE_COMPOSITOR)
    {
        Compositor *lastCompositor = nullptr;
        for (int i = 0; i < COMPOSITOR_COUNT; i++)
        {
            ShaderID shaderID = (ShaderID)(SHADER_COMPOSITE1 + i);
            Shader *s = getShader(shaderID);
            s->use();

            if (shaderID == SHADER_COMPOSITE3)
                s->setBool("uEnableFXAA", _fxaa);

            s->setBool("uWireframe", _wireframe);
            s->setTexture("uNoiseTex", _noiseTex, 5);
            s->setInt("uFrame", _frame);
            s->setFloat("uTime", _startFrameTime);

            s->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
            s->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

            Compositor *c = _compositors[i];
            c->bind();

            c->render(s, _gbuffer, lastCompositor);

            lastCompositor = c;
        }

        /* Bloom */
        _bloom->render(lastCompositor->getTexture(0));

        if (_visualizeMode == VISUALIZE_NONE)
        {
            /* Final composite render */

            Shader *s = getShader(SHADER_FINAL);
            s->use();

            s->setBool("uWireframe", _wireframe);
            s->setTexture("uNoiseTex", _noiseTex, 5);
            s->setInt("uFrame", _frame);
            s->setFloat("uTime", _startFrameTime);

            s->setTexture("uTexture0", lastCompositor->getTexture(0), 0);
            s->setTexture("uTexture1", _bloom->texture(), 1);

            s->setFloat("uGamma", _gamma);
            s->setFloat("uExposure", _exposure);
            s->setFloat("uBloomStrength", _bloomStrength);

            s->setCubemap("uSkybox", _skybox->skybox(), SKYBOX_TEXTURE_UNIT);
            s->setCubemap("uIrradiance", _skybox->irradiance(), IRRADIANCE_TEXTURE_UNIT);

            _final->bind();
            _final->render(s, _gbuffer, lastCompositor);
        }
        else
        {
            Shader *visualizeShader = getShader(SHADER_VISUALIZE);
            visualizeShader->use();
            visualizeShader->setInt("uMode", _visualizeMode);

            if (_visualizeCompositor >= 0 && _visualizeCompositor < COMPOSITOR_COUNT)
            {
                GLuint tex = _compositors[_visualizeCompositor]->getTexture(0);
                visualizeShader->setTexture("uTexture0", tex, 0);
            }
            else
				visualizeShader->setTexture("uTexture0", 0, 0);
            
			_visualizer->bind();
			_visualizer->render(visualizeShader, _gbuffer, nullptr);
        }
    }
    else
    {
        Shader *visualizeShader = getShader(SHADER_VISUALIZE);
        visualizeShader->use();
        visualizeShader->setInt("uMode", _visualizeMode);

        _visualizer->bind();
        _visualizer->render(visualizeShader, _gbuffer, nullptr);
    }
}

void endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(_window);

    _frame++;
}

Shader *getShader(ShaderID id)
{
    if (id < 0 || id >= SHADER_COUNT)
        return nullptr;
    return _shaders[id];
}

Gbuffer *getGbuffer()
{
    return _gbuffer;
}

Compositor *getCompositor(CompositorID id)
{
    if (id < 0 || id >= COMPOSITOR_COUNT)
        return nullptr;
}

Bloom *getBloom()
{
    return _bloom;
}

Camera *getCamera()
{
    return _camera;
}

const std::vector<RenderableMesh *> &getMeshes()
{
    return _meshes;
}

void addMesh(RenderableMesh *mesh)
{
    if (mesh)
    {
        mesh->retain();
        _meshes.push_back(mesh);
    }
}

const std::vector<Terrain *> &getTerrains()
{
    return _terrains;
}

void addTerrain(Terrain *terrain)
{
    if (terrain)
    {
        terrain->retain();
        _terrains.push_back(terrain);
    }
}

Terrain *getWater()
{
    return _water;
}

Skybox *getSkybox()
{
    return _skybox;
}

Generator *getTerrainGenerator()
{
    return _generator;
}

Mesh *getCubeMesh()
{
    return _cube;
}

VisualizeMode getVisualizeMode()
{
    return _visualizeMode;
}

void setVisualizeMode(VisualizeMode mode)
{
    _visualizeMode = mode;
}

void setVisualizeCompositor(CompositorID id)
{
    _visualizeCompositor = id;
}

bool getWireframe()
{
    return _wireframe;
}

void setWireframe(bool enabled)
{
    _wireframe = enabled;
}

float getExposure()
{
    return _exposure;
}

void setExposure(float exposure)
{
    _exposure = exposure;
}

float getGamma()
{
    return _gamma;
}

void setGamma(float gamma)
{
    _gamma = gamma;
}

float getBloomStrength()
{
    return _bloomStrength;
}

void setBloomStrength(float strength)
{
    _bloomStrength = strength;
}

bool getVsync()
{
    return _vsync;
}

void setVsync(bool enabled)
{
    if (_vsync != enabled)
    {
        _vsync = enabled;
        SDL_GL_SetSwapInterval(_vsync ? 1 : 0);
    }
}

bool getFXAAEnabled()
{
    return _fxaa;
}

void setFXAAEnabled(bool enabled)
{
    _fxaa = enabled;
}

GLuint getNoise()
{
    return _noiseTex;
}
