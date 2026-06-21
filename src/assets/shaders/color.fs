#version 330 core
out vec4 FragColor;

in vec3 vColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform sampler2D texture1;
uniform bool useTexture;

void main() {
    vec3 texColor = useTexture ? vec3(texture(texture1, TexCoords)) : vColor;

    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * texColor;
    FragColor = vec4(result, 1.0);
}
