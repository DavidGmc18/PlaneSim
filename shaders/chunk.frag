#version 460 core

in vec3 FragPos;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(uTexture, FragPos.xz);
}