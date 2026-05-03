#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec3 Norm;
in vec3 FragPos;

uniform vec3 cameraPos;

struct ParallelLight {
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform ParallelLight parallelLight;

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
    float opacity;
};

uniform Material material;

vec3 computeParallelLigth(ParallelLight light, vec3 norm, vec3 viewDir) {
    vec3 reflectDir = reflect(-light.dir, norm);

    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, uv).rgb;

    // Diffuse
    float diff = max(dot(norm, light.dir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, uv).rgb;

    // Specular
    float spec = material.shininess > 0.0f ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0f;
    vec3 specular = light.specular * spec * texture(material.specular, uv).rgb;

    return ambient + diffuse + specular;
}

vec3 computeLigth(Light light, vec3 norm, vec3 viewDir) {
    vec3 lightDir = normalize(light.pos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.0075f * pow(distance, 2));
    if (attenuation < (1.0f / 256.0f)) return vec3(0.0f);

    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, uv).rgb;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, uv).rgb;

    // Specular
    float spec = material.shininess > 0.0f ? pow(max(dot(norm, halfwayDir), 0.0), material.shininess) : 0.0f;
    vec3 specular = light.specular * spec * texture(material.specular, uv).rgb;

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 norm = normalize(Norm);
    // vec3 norm = normalize(texture(material.normal, uv).rgb * 2.0 - 1.0);

    vec3 color = computeParallelLigth(parallelLight, norm, viewDir);
    
    for (int i = 0; i < lightCount; i++) {
        color += computeLigth(lights[i], norm, viewDir);
    }
    FragColor = vec4(color, material.opacity);
}