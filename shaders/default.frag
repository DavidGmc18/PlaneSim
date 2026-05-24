#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec3 FragPos;
in mat3 TBN;

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
    sampler2D glossiness;
    float opacity;
};

uniform Material material;

vec3 computeParallelLigth(ParallelLight light, vec3 norm, vec3 viewDir, vec3 tex_diffuse, vec3 tex_specular, float glossiness) {
    vec3 reflectDir = reflect(-light.dir, norm);

    // Ambient
    vec3 ambient = light.ambient * tex_diffuse;

    // Diffuse
    float diff = max(dot(norm, light.dir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex_diffuse;

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), glossiness);
    vec3 specular = light.specular * spec * tex_specular;

    return ambient + diffuse + specular;
}

vec3 computeLigth(Light light, vec3 norm, vec3 viewDir, vec3 tex_diffuse, vec3 tex_specular, float glossiness) {
    vec3 lightDir = normalize(light.pos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Attenuation
    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (1.0f + 0.0075f * pow(distance, 2));
    if (attenuation < (1.0f / 256.0f)) return vec3(0.0f);

    // Ambient
    vec3 ambient = light.ambient * tex_diffuse;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex_diffuse;

    // Specular
    float spec = pow(max(dot(norm, halfwayDir), 0.0), glossiness);
    vec3 specular = light.specular * spec * tex_specular;

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 norm = texture(material.normal, uv).xyz;
    norm = norm * 2.0 - 1.0;   
    norm = normalize(TBN * norm);
    norm = gl_FrontFacing ? norm : -norm;

    vec3 tex_diffuse = texture(material.diffuse, uv).rgb;
    vec3 tex_specular = texture(material.specular, uv).rgb;
    
    float glossiness = pow(2, 8 * texture(material.glossiness, uv).r);
    
    vec3 viewDir = normalize(cameraPos - FragPos);

    vec3 color = computeParallelLigth(parallelLight, norm, viewDir, tex_diffuse, tex_specular, glossiness);
    for (int i = 0; i < lightCount; i++) {
        color += computeLigth(lights[i], norm, viewDir, tex_diffuse, tex_specular, glossiness);
    }
    FragColor = vec4(color, material.opacity);
}