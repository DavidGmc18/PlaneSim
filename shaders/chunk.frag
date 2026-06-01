#version 460 core

in vec2 uv;
uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(uTexture, uv);
}