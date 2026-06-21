#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in mat4 instanceModel;

out vec3 vColor;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform int useInstancing;
uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 finalModel = (useInstancing == 1) ? instanceModel : model;
    FragPos = vec3(finalModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(finalModel))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * finalModel * vec4(aPos, 1.0);
    vColor = aColor;
}
