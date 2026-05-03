#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec2 uv;
out vec3 Norm;
out vec3 FragPos;

void main() {
    vec4 pos = uModel * vec4(aPos, 1.0);

    gl_Position = uProjection * uView * pos;
    uv = aUV;
    Norm = mat3(transpose(inverse(uModel))) * aNorm; 
    FragPos = vec3(pos);
}