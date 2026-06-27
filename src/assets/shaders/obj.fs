#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float reflectStrength;

uniform sampler2D matTex0;
uniform sampler2D matTex1;
uniform sampler2D matTex2;
uniform sampler2D matTex3;
uniform sampler2D matTex4;
uniform sampler2D matTex5;

uniform vec4 matColor0;
uniform vec4 matColor1;
uniform vec4 matColor2;
uniform vec4 matColor3;
uniform vec4 matColor4;
uniform vec4 matColor5;

void main() {
    vec3 kd = matColor0.rgb;
    vec3 ka = matColor1.rgb;

    vec3 texDiffuse = texture(matTex0, TexCoords).rgb;
    if (texDiffuse != vec3(0.0))
        kd = texDiffuse;

    vec3 ambient = ambientStrength * ka;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * kd * reflectStrength;

    FragColor = vec4(ambient + diffuse, 1.0);
}
