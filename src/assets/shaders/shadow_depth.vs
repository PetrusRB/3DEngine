#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in mat4 instanceModel;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform int useInstancing;

void main() {
    mat4 finalModel = (useInstancing == 1) ? instanceModel : model;
    gl_Position = lightSpaceMatrix * finalModel * vec4(aPos, 1.0);
}
