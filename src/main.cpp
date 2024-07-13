#include "engine.h"

#include "camera.h"
#include "skybox.h"
#include "mesh.h"
#include "util.h"

#include <imgui.h>

static constexpr float kMoveSpeed = 4.0f;
static constexpr float kTurnSpeed = 90.0f;

static void debugWindow();

int main(int argc, char *argv[])
{
    initAll(argc, argv);

    Skybox *skybox = getSkybox();
    skybox->setSunAltitude(25.0f);
    skybox->setSunAzimuth(15.0f);
    skybox->setSunColor(Vector3(1.0f));
    skybox->setSunIntensity(1.0f);
    skybox->setSunTightness(500.0f);

    skybox->setHorizonColor(colorRGB(135, 206, 235));
    skybox->setZenithColor(colorRGB(70, 130, 180));

    RenderableMesh *cube = new RenderableMesh(getCubeMesh());
    addMesh(cube);

    cube->setScale(Vector3(10.0f, 1.0f, 10.0f));

    Material *material = cube->getMaterial();
    material->albedoColor = colorRGB(255, 127, 127);
    material->roughnessValue = 0.5f;
    material->aoValue = 1.0f;

    while (beginFrame())
    {
        float dt = deltaTime();
        Camera *camera = getCamera();

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
        if (getKey(SDL_SCANCODE_W))
            position += kMoveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_S))
            position -= kMoveSpeed * dt * camera->front();
        if (getKey(SDL_SCANCODE_A))
            position -= kMoveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_D))
            position += kMoveSpeed * dt * camera->right();
        if (getKey(SDL_SCANCODE_E))
            position += kMoveSpeed * dt * kWorldUp;
        if (getKey(SDL_SCANCODE_Q))
            position -= kMoveSpeed * dt * kWorldUp;
        camera->setPosition(position);

        // printf("(%f, %f, %f) (%f, %f)\n", position.x, position.y, position.z, pitch, yaw);

        debugWindow();

        renderAll();
        endFrame();
    }

    cube->release();

    quitAll();
    return 0;
}

static void debugWindow()
{
    ImGui::Begin("Debug");

    ImGui::SeparatorText("Camera");

    Camera *camera = getCamera();
    ImGui::InputFloat3("Position", (float *)&camera->position(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Pitch", (float *)camera->getPitch(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Yaw", (float *)camera->getYaw(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Near", (float *)camera->getNear(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat("Far", (float *)camera->getFar(), 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Front", (float *)&camera->front(), "%.3f", ImGuiInputTextFlags_ReadOnly);

    ImGui::SeparatorText("Visualizer");

    static int visualizeMode = VISUALIZE_NONE;
    ImGui::SliderInt("Mode", &visualizeMode, VISUALIZE_NONE, VISUALIZE_MATERIAL);
    setVisualizeMode((VisualizeMode)visualizeMode);

    ImGui::End();
}
