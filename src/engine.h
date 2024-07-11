#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <mutil/mutil.h>
#include <lysys/lysys.hpp>

using namespace mutil;

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

bool beginFrame();
void updateAll();
void endFrame();
