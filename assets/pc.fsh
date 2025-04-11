#version 460 core

in vec4 vertexColor;
in vec3 fragCoord;

out vec4 fragColor;

void main() {
    // fragColor = vec4(1.0 - vertexColor.xyz, vertexColor.a);
    fragColor = vertexColor;
}