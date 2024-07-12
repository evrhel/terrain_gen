layout (std140) uniform Camera
{
    vec3 position;
    vec3 direction;

    float near;
    float far;

    vec3 front;
    vec3 right;
    vec3 up;

    mat4 view;
    mat4 invView;

    mat4 proj;
    mat4 invProj;

    mat4 viewProj;
    mat4 invViewProj;
} uCamera;
