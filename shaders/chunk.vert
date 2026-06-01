#version 460 core

layout (location = 0) in vec2 aXZ;
layout (location = 1) in float aY;

uniform vec2 uOffset; // instead of model matrix
uniform mat4 uVP;

out vec3 FragPos;

void main() {
    vec2 pos_xz = uOffset + aXZ;
    FragPos = vec3(pos_xz.x, aY, pos_xz.y);

    gl_Position = uVP *  vec4(FragPos, 1.0);
}