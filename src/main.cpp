#include "engine.h"

#include "camera.h"
#include "skybox.h"

static constexpr Vector3 kMoveSpeed = Vector3(1.0f);
static constexpr float kTurnSpeed = 45.0f;

int main(int argc, char *argv[])
{
    initAll(argc, argv);

    Skybox *skybox = getSkybox();
    skybox->setSunAltitude(45.0f);
    skybox->setSunAzimuth(0.0f);
    skybox->setSunColor(Vector3(1.0f));
    skybox->setSunIntensity(1.0f);
    skybox->setSunTightness(500.0f);

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
            yaw -= kTurnSpeed * dt;
        if (getKey(SDL_SCANCODE_RIGHT))
            yaw += kTurnSpeed * dt;
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
        camera->setPosition(position);

        printf("(%f, %f, %f) (%f, %f)\n", position.x, position.y, position.z, pitch, yaw);

        renderAll();
        endFrame();
    }

    quitAll();
    return 0;
}
