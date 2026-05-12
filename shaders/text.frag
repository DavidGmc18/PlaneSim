#version 460

in vec2 uv;

out vec4 FragColor;

uniform sampler2D uAtlas;
uniform vec4 uColor;

void main() {
    vec4 color = uColor;
    color.a *= texture(uAtlas, uv).r;
    FragColor = color;
}