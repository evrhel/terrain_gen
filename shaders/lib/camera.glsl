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

    mat4 projView;
    mat4 invProjView;

    mat4 normalMatrix;
    mat4 invNormalMatrix;
} uCamera;
