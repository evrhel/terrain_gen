#include "engine.h"

#include <cstdarg>
#include <cstdio>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "mesh.h"

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

static float _startTime;

static float _startFrameTime;
static float _deltaTime;

static bool _keys[SDL_NUM_SCANCODES];
static bool _mouseButtons[8];
static IntVector2 _mousePosition;

static std::vector<Mesh *> _meshes;

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

void initAll(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        fatal("SDL_Init: %s\n", SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    _window = SDL_CreateWindow(
        "Terrain Generator",
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window)
        fatal("SDL_CreateWindow: %s\n", SDL_GetError());

    _gl = SDL_GL_CreateContext(_window);
    if (!_gl)
        fatal("SDL_GL_CreateContext: %s\n", SDL_GetError());

    if (!gladLoadGLLoader((void *(*)(const char *))SDL_GL_GetProcAddress))
        fatal("gladLoadGLLoader failed\n");

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(_window, _gl);
    ImGui_ImplOpenGL3_Init();

    createQuad();

    printf("OpenGL : %s\n", glGetString(GL_VERSION));
    printf("GLSL   : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor : %s\n", glGetString(GL_VENDOR));

    _startTime = ls_time();
    _startFrameTime = _startTime;
}

void quitAll()
{
    for (Mesh *mesh : _meshes)
        delete mesh;
    _meshes.clear();

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
    return ls_time() - _startTime;
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
    float time = ls_time();

    _deltaTime = time - _startFrameTime;
    _startFrameTime = time;

    return pollEvents();
}

void updateAll()
{
}

void endFrame()
{
    SDL_GL_SwapWindow(_window);
}

const std::vector<Mesh *> &getMeshes()
{
    return _meshes;
}

void addMesh(Mesh *mesh)
{
    _meshes.push_back(mesh);
}
