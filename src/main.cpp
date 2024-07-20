#include "engine.h"

#include "camera.h"
#include "skybox.h"
#include "mesh.h"
#include "util.h"
#include "terrain.h"
#include "bloom.h"
#include "shader.h"

#include <imgui.h>

static constexpr float kMoveSpeed = 16.0f;
static constexpr float kShiftMultiplier = 10.0f;
static constexpr float kTurnSpeed = 90.0f;

static constexpr float kSunAltitude = 25.0f;
static constexpr float kSunAzimuth = 15.0f;
static constexpr Vector3 kSunColor = Vector3(1.0f, 1.0f, 0.82f);
static constexpr float kSunIntensity = 1.0f;
static constexpr float kSunTightness = 500.0f;
static constexpr Vector3 kHorizonColor = colorRGB(135, 206, 235);
static constexpr Vector3 kZenithColor = colorRGB(70, 130, 180);

static constexpr int kTerrainSize = 4096;

static Terrain *terrain;

static void debugWindow();
static void initTerrainMaterials();
static void initWater();

int main(int argc, char *argv[])
{
    initAll(argc, argv);

    terrain = new Terrain();
    //terrain->load(kTerrainSize, kTerrainSize, 20, 128.0f);
    terrain->load("assets/terrain", kTerrainSize, kTerrainSize, 20, 4.0f);
    addTerrain(terrain);

    initTerrainMaterials();

    initWater();

    Camera *camera = getCamera();
    camera->setFar(2048.0f);

    while (beginFrame())
    {
        float dt = deltaTime();

        float pitch = camera->pitch();
        if (getKey(SDL_SCANCODE_UP))
            pitch += kTurnSpeed * dt;
        if (getKey(SDL_SCANCODE_DOWN))
            pitch -= kTurnSpeed * dt;
        pitch = mutil::clamp(pitch, -89.0f, 89.0f);
        camera->setPitch(pitch);

        float yaw = camera->yaw();
        if (getKey(SDL_SCANCODE_LEFT))
            yaw += kTurnSpeed * dt;
        if (getKey(SDL_SCANCODE_RIGHT))
            yaw -= kTurnSpeed * dt;
        camera->setYaw(yaw);

        Vector3 position = camera->position();
        float moveSpeed = getKey(SDL_SCANCODE_LSHIFT) ? kMoveSpeed * kShiftMultiplier : kMoveSpeed;
        if (getKey(SDL_SCANCODE_W))
            position += moveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_S))
            position -= moveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_A))
            position -= moveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_D))
            position += moveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_E))
            position += moveSpeed * dt * kWorldUp;
        if (getKey(SDL_SCANCODE_Q))
            position -= moveSpeed * dt * kWorldUp;
        camera->setPosition(position);

        Shader *perlinShader = getShader(SHADER_PERLIN);
        perlinShader->use();

        debugWindow();

        renderAll();
        endFrame();
    }

    terrain->release();

    quitAll();
    return 0;
}

static void debugWindow()
{
    static int visualizeMode = VISUALIZE_NONE;
    static bool wireframe = false;

    static float sunAltitude = kSunAltitude;
    static float sunAzimuth = kSunAzimuth;
    static Vector3 sunColor = kSunColor;
    static float sunIntensity = kSunIntensity;
    static float sunTightness = kSunTightness;
    static Vector3 horizonColor = kHorizonColor;
    static Vector3 zenithColor = kZenithColor;

    static float exposure = 1.0f;
    static float gamma = 2.2f;

    Skybox *skybox = getSkybox();

    ImGui::Begin("Debug");

    if (ImGui::BeginTabBar("Debug"))
    {
        if (ImGui::BeginTabItem("General"))
        {
            ImGui::SeparatorText("Performance");

            ImGui::LabelText("Frame Time", "%.3f ms", deltaTime() * 1000.0f);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera"))
        {
            ImGui::SeparatorText("Camera");

            Camera *camera = getCamera();
            ImGui::InputFloat3("Position", (float *)&camera->position(), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Pitch", (float *)camera->getPitch(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Yaw", (float *)camera->getYaw(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Near", (float *)camera->getNear(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat("Far", (float *)camera->getFar(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat3("Front", (float *)&camera->front(), "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::SeparatorText("Debug");

            ImGui::SliderInt("Buffer", &visualizeMode, VISUALIZE_NONE, VISUALIZE_MATERIAL);

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
            ImGui::SliderFloat("Intensity", &sunIntensity, 0.0f, 10.0f);

            ImGui::PopID();

            ImGui::PushID("Sky");
            ImGui::SeparatorText("Sky");

            ImGui::ColorEdit3("Horizon Color", (float *)&horizonColor);
            ImGui::ColorEdit3("Zenith Color", (float *)&zenithColor);
            ImGui::SliderFloat("Sun Tightness", &sunTightness, 0.0f, 1000.0f);

            ImGui::PopID();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Postprocess"))
        {
            ImGui::SeparatorText("Tonemapping");
            ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);

            ImGui::SeparatorText("Display");
            ImGui::SliderFloat("Gamma", &gamma, 0.1f, 10.0f);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    setVisualizeMode((VisualizeMode)visualizeMode);
    setWireframe(wireframe);

    skybox->setSunAltitude(sunAltitude);
    skybox->setSunAzimuth(sunAzimuth);
    skybox->setSunColor(sunColor);
    skybox->setSunIntensity(sunIntensity);
    skybox->setSunTightness(sunTightness);
    skybox->setHorizonColor(horizonColor);
    skybox->setZenithColor(zenithColor);

    setExposure(exposure);
    setGamma(gamma);
}

static void initTerrainMaterials()
{
    Material *materials = terrain->getMaterials();
    Material &dirt = materials[TERRAIN_DIRT_INDEX];
    Material &grass = materials[TERRAIN_GRASS_INDEX];
    Material &snow = materials[TERRAIN_SNOW_INDEX];
    Material &rock = materials[TERRAIN_ROCK_INDEX];
    Material &sand = materials[TERRAIN_SAND_INDEX];

    dirt.load("assets/rocky_dirt1");
    grass.load("assets/forest-floor");
    snow.load("assets/snowdrift1");
    rock.load("assets/jagged-rocky-ground1");
    sand.load("assets/wavy-sand");
}

static void initWater()
{
    Terrain *water = getWater();
    water->load(kTerrainSize, kTerrainSize, 10, 0.5f);

    Material *material = water->getMaterial();
    material->normal.load("assets/water.jpg");

    water->setEnabled(true);
}
