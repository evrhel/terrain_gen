#include "engine.h"

#include "camera.h"
#include "skybox.h"
#include "mesh.h"
#include "util.h"
#include "terrain.h"
#include "bloom.h"
#include "shader.h"
#include "generator.h"

#include <imgui.h>

static constexpr float kMinMoveSpeed = 0.0f;
static constexpr float kMaxMoveSpeed = 100.0f;
static float moveSpeed = 16.0f;

static constexpr float kShiftMultiplier = 10.0f;
static constexpr float kTurnSpeed = 90.0f;

static constexpr float kIntrinsicSensitivity = 1.0f / 20.0f;
static float mouseSensitivity = 2.5f;
static float fov = 45.0f;
static float near = 0.1f;
static float far = 2048.0f;

static constexpr float kSunAltitude = 25.0f;
static constexpr float kSunAzimuth = 15.0f;
static constexpr Vector3 kSunColor = Vector3(1.0f, 1.0f, 0.82f);
static constexpr float kSunIntensity = 8.0f;
static constexpr float kSunTightness = 650.0f;
static constexpr float kFogDensity = 0.01f;

// Earth: 6.371e6m, 1.2e5m
// Mars: 3.3895e6m, 1e5m
// Moon: 1.7374e6m, 1e2m

static constexpr float kPlanetRadius = 6.371e6f;
static constexpr float kAtmosphereRadius = kPlanetRadius + 1.2e5f;
static constexpr float kHr = 7994.0f;
static constexpr float kHm = 1200.0f;
static constexpr float kMiePhase = 0.76f;

static constexpr int kTerrainSize = 4096;

//static Terrain *terrain;

static void debugWindow();
static void initTerrainMaterials();
static void initWater();

int main(int argc, char *argv[])
{
    initAll(argc, argv);

    //terrain = new Terrain();
    //terrain->load("assets/terrain", 20);

    //Vector3 terrainScale = Vector3(kTerrainSize / terrain->width(), 1.0f, kTerrainSize / terrain->height());
    //terrain->setScale(terrainScale);

    //addTerrain(terrain);

    initTerrainMaterials();

    initWater();

    Camera *camera = getCamera();
    camera->setFov(fov);
    camera->setNear(near);
    camera->setFar(far);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool lastEscape = false, lastF11 = false;
    bool mouseLocked = true, fullscreen = false;

    while (beginFrame())
    {
        bool escape = getKey(SDL_SCANCODE_ESCAPE);
        bool f11 = getKey(SDL_SCANCODE_F11);

        if (!lastEscape && escape)
        {
            mouseLocked = !mouseLocked;
            SDL_SetRelativeMouseMode(mouseLocked);
        }

        if (!lastF11 && f11)
        {
            fullscreen = !fullscreen;

            SDL_Window *window = getWindow();
            SDL_DisplayID display = SDL_GetDisplayForWindow(window);
            const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display);


            if (fullscreen)
            {
                SDL_SetWindowSize(window, mode->w, mode->h);
                SDL_SetWindowFullscreen(window, SDL_TRUE);
            }
            else
            {
                int width = mode->w * 2 / 3;
                int height = mode->h * 2 / 3;

                SDL_SetWindowSize(window, width, height);
                SDL_SetWindowFullscreen(window, SDL_FALSE);
                SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            }
        }

        lastF11 = f11;
        lastEscape = escape;

        float dt = deltaTime();

        float pitch = camera->pitch();
        float yaw = camera->yaw();

        if (mouseLocked)
        {
            const IntVector2 &mouseDelta = getMouseDelta();
            pitch -= mouseSensitivity * mouseDelta.y * kIntrinsicSensitivity;
            yaw -= mouseSensitivity * mouseDelta.x * kIntrinsicSensitivity;
        }

        if (getKey(SDL_SCANCODE_UP))
            pitch += kTurnSpeed * dt;
        if (getKey(SDL_SCANCODE_DOWN))
            pitch -= kTurnSpeed * dt;

        if (getKey(SDL_SCANCODE_LEFT))
            yaw += kTurnSpeed * dt;
        if (getKey(SDL_SCANCODE_RIGHT))
            yaw -= kTurnSpeed * dt;
        
        pitch = mutil::clamp(pitch, -89.0f, 89.0f);
        camera->setPitch(pitch);
        camera->setYaw(yaw);

        const IntVector2 &scroll = getScroll();
        moveSpeed = mutil::clamp(moveSpeed + scroll.y * 2, kMinMoveSpeed, kMaxMoveSpeed);

        Vector3 position = camera->position();

        float actualMoveSpeed = moveSpeed;
        if (getKey(SDL_SCANCODE_LSHIFT))
            actualMoveSpeed *= kShiftMultiplier;

        if (getKey(SDL_SCANCODE_W))
            position += actualMoveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_S))
            position -= actualMoveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_A))
            position -= actualMoveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_D))
            position += actualMoveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_E))
            position += actualMoveSpeed * dt * kWorldUp;
        if (getKey(SDL_SCANCODE_Q))
            position -= actualMoveSpeed * dt * kWorldUp;
        camera->setPosition(position);

        debugWindow();

        renderAll();
        endFrame();
    }

    //terrain->release();

    quitAll();
    return 0;
}

static void debugWindow()
{
    static bool vsync = true;

    static int visualizeMode = VISUALIZE_NONE;
    static int compositor = COMPOSITOR1;
    static bool wireframe = false;

    static float sunAltitude = kSunAltitude;
    static float sunAzimuth = kSunAzimuth;
    static Vector3 sunColor = kSunColor;
    static float sunIntensity = kSunIntensity;
    static float sunTightness = kSunTightness;
    static float fogDensity = kFogDensity;
    static float planetRadius = kPlanetRadius;
    static float atmosphereRadius = kAtmosphereRadius;
    static float Hr = kHr;
    static float Hm = kHm;
    static float miePhase = kMiePhase;

    static float exposure = 1.0f;
    static float bloomStrength = 0.2f;
    static float gamma = 2.2f;
    static bool fxaa = true;
    static TonemapMode tonemap = TONEMAP_ACES;

    Skybox *skybox = getSkybox();

    ImGui::Begin("Debug");

    if (ImGui::BeginTabBar("Debug"))
    {
        if (ImGui::BeginTabItem("General"))
        {
            ImGui::SeparatorText("Info");

            const IntVector2 &winSize = getWindowSize();
            double dt = (double)deltaTime();

            ImGui::LabelText("Resolution", "%dx%d", winSize.x, winSize.y);
            ImGui::LabelText("Framerate", "%.2f f/s (%.2f ms)", 1 / dt, dt * 1000);
            ImGui::LabelText("Device", (const char *)glGetString(GL_RENDERER));

            ImGui::SeparatorText("Options");

            ImGui::Checkbox("Vsync", &vsync);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera"))
        {
            Camera *camera = getCamera();

            ImGui::SeparatorText("Options");

            ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.1f, 10.0f);
            ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f);
            ImGui::SliderFloat("Near", &near, 0.1f, 1.0f);
            ImGui::SliderFloat("Far", &far, 1.0f, 10000.0f);
            ImGui::SliderFloat("Move Speed", &moveSpeed, kMinMoveSpeed, kMaxMoveSpeed);

            camera->setFov(fov);
            camera->setNear(near);
            camera->setFar(far);

            ImGui::SeparatorText("Camera");

            ImGui::InputFloat3("Position", (float *)&camera->position(), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Pitch", (float *)camera->getPitch(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Yaw", (float *)camera->getYaw(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat3("Front", (float *)&camera->front(), "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::SeparatorText("Debug");

            ImGui::SliderInt("Buffer", &visualizeMode, VISUALIZE_NONE, VISUALIZE_COMPOSITOR);
            ImGui::SliderInt("Compositor", &compositor, COMPOSITOR1, COMPOSITOR_COUNT - 1);

            ImGui::Checkbox("Wireframe", &wireframe);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Atmosphere"))
        {
            ImGui::PushID("Sun");
            ImGui::SeparatorText("Sun");

            ImGui::SliderFloat("Altitude", &sunAltitude, 0.0f, 360.0f);
            ImGui::SliderFloat("Azimuth", &sunAzimuth, 0.0f, 360.0f);
            ImGui::ColorEdit3("Color", (float *)&sunColor);
            ImGui::SliderFloat("Intensity", &sunIntensity, 0.0f, 50.0f);

            ImGui::InputFloat3("Direction", (float *)&skybox->sunDirection(), "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::PopID();

            ImGui::PushID("Sky");
            ImGui::SeparatorText("Sky");
            
            ImGui::SliderFloat("Sun Tightness", &sunTightness, 0.0f, 1500.0f);

            ImGui::PopID();

            ImGui::PushID("Volumetrics");
            ImGui::SeparatorText("Volumetrics");

            ImGui::SliderFloat("Fog Density", &fogDensity, 0.0f, 1.0f);

            ImGui::PopID();

            ImGui::PushID("Planet");

            ImGui::SeparatorText("Planet");

            ImGui::InputFloat("Planet Radius", &planetRadius, 0.0f, 0.0f, "%.0f m");
            ImGui::SetItemTooltip("Radius of the planet in meters.");

            ImGui::InputFloat("Atmosphere Radius", &atmosphereRadius, 0.0f, 0.0f, "%.0f m");
            ImGui::SetItemTooltip("Radius of the atmosphere in meters.");

            ImGui::SliderFloat("Rayleigh Scale Height", &Hr, 0.0f, 10000.0f);
            ImGui::SetItemTooltip("Rayleigh scale height in meters.");

            ImGui::SliderFloat("Mie Scale Height", &Hm, 0.0f, 10000.0f);
            ImGui::SetItemTooltip("Mie scale height in meters.");

            ImGui::SliderFloat("Mie Scattering Coefficient", &miePhase, -1.0f, 1.0f);
            ImGui::SetItemTooltip("Mie phase function.");

            ImGui::PopID();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Postprocess"))
        {
            ImGui::SeparatorText("Antialiasing");
            ImGui::Checkbox("FXAA", &fxaa);

            ImGui::SeparatorText("Tonemapping");

            static const char *kTonemapNames[] = {
				"Linear",
				"Reinhard",
				"ACES",
                "Uncharted 2 (RGB)",
                "Uncharted 2 (Luminance)"
			};

            if (ImGui::BeginCombo("Tonemap Function", kTonemapNames[tonemap]))
            {
                for (int i = 0; i < 5; i++)
                {
                    bool selected = tonemap == i;
                    if (ImGui::Selectable(kTonemapNames[i], selected))
                        tonemap = (TonemapMode)i;

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);

            ImGui::SeparatorText("Bloom");
            ImGui::SliderFloat("Strength", &bloomStrength, 0.0f, 1.0f);

            ImGui::SeparatorText("Display");
            ImGui::SliderFloat("Gamma", &gamma, 0.1f, 10.0f);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    setVsync(vsync);

    setVisualizeMode((VisualizeMode)visualizeMode);
    setVisualizeCompositor((CompositorID)compositor);
    setWireframe(wireframe);

    skybox->setSunAltitude(sunAltitude);
    skybox->setSunAzimuth(sunAzimuth);
    skybox->setSunColor(sunColor);
    skybox->setSunIntensity(sunIntensity);
    skybox->setSunTightness(sunTightness);
    skybox->setFogDensity(fogDensity);
    skybox->setPlanetRadius(planetRadius);
    skybox->setAtmosphereRadius(atmosphereRadius);
    skybox->setHr(Hr);
    skybox->setHm(Hm);
    skybox->setMiePhase(miePhase);

    setExposure(exposure);
    setGamma(gamma);
    setBloomStrength(bloomStrength);
    setFXAAEnabled(fxaa);
    setTonemapMode(tonemap);
}

static void initTerrainMaterials()
{
    TerrainMaterials &materials = getTerrainGenerator()->getMaterials();
    auto &dirt = materials[TERRAIN_DIRT_INDEX];
    auto &grass = materials[TERRAIN_GRASS_INDEX];
    auto &snow = materials[TERRAIN_SNOW_INDEX];
    auto &rock = materials[TERRAIN_ROCK_INDEX];
    auto &sand = materials[TERRAIN_SAND_INDEX];

    dirt = loadMaterial("assets/rocky_dirt1");
    grass = loadMaterial("assets/forest-floor");
    snow = loadMaterial("assets/snowdrift1");
    rock = loadMaterial("assets/jagged-rocky-ground1");
    sand = loadMaterial("assets/wavy-sand");
}

static void initWater()
{
    Terrain *water = getWater();
    water->load(kTerrainSize, kTerrainSize, 10);

    auto &material = water->getMaterial();
    material->normal = loadTexture2D("assets/water.jpg", COLOR_SPACE_LINEAR);

    water->setEnabled(true);
}
