#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec4 aTan;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;

out vec2 uv;
out vec3 FragPos;
out mat3 TBN;

void main() {
    vec4 pos = uModel * vec4(aPos, 1.0);
    gl_Position = uProjection * uView * pos;
    uv = aUV;
    FragPos = vec3(pos);

    vec3 T = normalize(uNormalMatrix * aTan.xyz);
    vec3 N = normalize(uNormalMatrix * aNorm);
    vec3 B = cross(N, T) * aTan.w;
    TBN = mat3(T, B, N);
}