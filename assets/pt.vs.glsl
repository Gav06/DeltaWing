#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;
out vec3 fragCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
   gl_Position = projection * model * vec4(aPos, 1.0);
   fragCoord = aPos;
   texCoord = aTex;
}