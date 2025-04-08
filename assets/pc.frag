#version 460 core

in vec4 vertexColor;
in vec3 fragCoord;

out vec4 fragColor;

void main() {
    fragColor = vertexColor;
}