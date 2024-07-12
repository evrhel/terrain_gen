#include "engine.h"

#include <cstdarg>
#include <cstdio>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "mesh.h"
#include "shader.h"
#include "skybox.h"
#include "camera.h"
#include "util.h"

static const Vector4 kQuadVertices[] = {
    Vector4(-1.0f, -1.0f, 0.0f, 0.0f),
    Vector4(1.0f, -1.0f, 1.0f, 0.0f),
    Vector4(1.0f, 1.0f, 1.0f, 1.0f),
    Vector4(-1.0f, 1.0f, 0.0f, 1.0f),
};

static const GLuint kQuadIndices[] = {
    0,
    1,
    2,
    2,
    3,
    0,
};

static SDL_Window *_window;
static SDL_GLContext _gl;

static GLuint _quadVao, _quadVbo, _quadEbo;

static IntVector2 _windowSize;

static double _startTime;

static float _startFrameTime;
static float _deltaTime;

static bool _keys[SDL_NUM_SCANCODES];
static bool _mouseButtons[8];
static IntVector2 _mousePosition;

static Shader *_shaders[SHADER_COUNT];

static Camera *_camera;

static std::vector<Mesh *> _meshes;
static Skybox *_skybox;

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
            _mousePosition.x = evt.motion.x;
            _mousePosition.y = evt.motion.y;
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
}

static void destroyQuad()
{
    glDeleteBuffers(1, &_quadEbo);
    glDeleteBuffers(1, &_quadVbo);
    glDeleteVertexArrays(1, &_quadVao);
}

/* Sources */
#include <shaders/composite1.frag.h>
#include <shaders/composite2.frag.h>
#include <shaders/generic.frag.h>
#include <shaders/generic.vert.h>
#include <shaders/screen.vert.h>
#include <shaders/skybox.frag.h>
#include <shaders/skybox.vert.h>

static void loadShaders()
{
    for (int32_t i = 0; i < SHADER_COUNT; i++)
        _shaders[i] = new Shader();

    /* Load shaders */

    Shader *composite1 = getShader(SHADER_COMPOSITE1);
    composite1->load("composite1", screen_vert_source, composite1_frag_source);

    Shader *composite2 = getShader(SHADER_COMPOSITE2);
    composite2->load("composite2", screen_vert_source, composite2_frag_source);

    Shader *generic = getShader(SHADER_GENERIC);
    generic->load("generic", generic_vert_source, generic_frag_source);

    Shader *skybox = getShader(SHADER_SKYBOX);
    skybox->load("skybox", skybox_vert_source, skybox_frag_source);
}

static void destroyShaders()
{
    for (int32_t i = 0; i < SHADER_COUNT; i++)
    {
        if (_shaders[i])
            delete _shaders[i];
    }
}

void initAll(int argc, char *argv[])
{
    /* Initialize SDL */

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        fatal("SDL_Init: %s\n", SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    /* Create a window */
    _window = SDL_CreateWindow(
        "Terrain Generator",
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window)
        fatal("SDL_CreateWindow: %s\n", SDL_GetError());

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

    /* Default quad */
    createQuad();

    /* Enable vsync */
    SDL_GL_SetSwapInterval(1);

    printf("OpenGL : %s\n", glGetString(GL_VERSION));
    printf("GLSL   : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor : %s\n", glGetString(GL_VENDOR));

    /* Load shaders */
    loadShaders();

    /* Setup camera */
    _camera = new Camera();
    _camera->load();

    /* Create skybox */
    _skybox = new Skybox();
    _skybox->load();

    /* Initialize time */
    _startTime = ls_time64();
    _startFrameTime = _startTime;
}

void quitAll()
{
    /* Destroy in reverse order */

    for (Mesh *mesh : _meshes)
        delete mesh;
    _meshes.clear();

    delete _skybox;

    delete _camera;

    destroyShaders();

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

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    return pollEvents();
}

void renderAll()
{
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Update */
    _camera->update();
    _skybox->update();

    Shader *genericShader = getShader(SHADER_GENERIC);
    genericShader->use();

    /* Render meshes */
    glDepthFunc(GL_LESS);
    for (Mesh *mesh : _meshes)
    {
        if (mesh->enabled())
        {
            mesh->update();
            genericShader->setMatrix4("uModel", Matrix4(1.0f));
            mesh->render();
        }
    }

    Shader *skyboxShader = getShader(SHADER_SKYBOX);
    skyboxShader->use();

    /* Render skybox */
    glDepthFunc(GL_LEQUAL);
    _skybox->render();
}

void endFrame()
{
    SDL_GL_SwapWindow(_window);
}

Shader *getShader(ShaderID id)
{
    if (id < 0 || id >= SHADER_COUNT)
        return nullptr;
    return _shaders[id];
}

Camera *getCamera()
{
    return _camera;
}

const std::vector<Mesh *> &getMeshes()
{
    return _meshes;
}

void addMesh(Mesh *mesh)
{
    _meshes.push_back(mesh);
}

Skybox *getSkybox()
{
    return _skybox;
}
