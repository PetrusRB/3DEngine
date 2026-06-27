#version 330 core
out vec4 FragColor;

in vec3 vColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_DIR_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform float reflectStrength;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform float shininess;
uniform sampler2D texture1;
uniform bool useTexture;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    return light.ambient * texColor + (light.diffuse * diff + light.specular * spec * reflectStrength) * texColor;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    vec3 ambient = light.ambient * texColor * attenuation;
    vec3 diffuse = light.diffuse * diff * texColor * attenuation;
    vec3 specular = light.specular * spec * reflectStrength * attenuation;
    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient * texColor * attenuation;
    vec3 diffuse = light.diffuse * diff * texColor * attenuation * intensity;
    vec3 specular = light.specular * spec * reflectStrength * attenuation * intensity;
    return ambient + diffuse + specular;
}

void main() {
    vec3 texColor = useTexture ? vec3(texture(texture1, TexCoords)) : vColor;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);  

    vec3 result = vec3(0.0);

    for (int i = 0; i < numDirLights && i < MAX_DIR_LIGHTS; i++)
        result += CalcDirLight(dirLights[i], norm, viewDir, texColor);

    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texColor);

    for (int i = 0; i < numSpotLights && i < MAX_SPOT_LIGHTS; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, texColor);

    float dist = length(viewPos - FragPos);
    float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
    result = mix(fogColor, result, fogFactor);
    result = pow(result, vec3(1.0 / 2.2));
    FragColor = vec4(result, 1.0);
}
