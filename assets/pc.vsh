#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 projection;
uniform mat4 model;

out vec4 vertexColor;
out vec3 fragCoord;

void main() {
   gl_Position = projection * model * vec4(aPos, 1.0);
   fragCoord = aPos;
   vertexColor = aColor;
}