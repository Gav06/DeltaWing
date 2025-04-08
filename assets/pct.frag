#version 460 core

in vec2 texCoord;
in vec4 vertexColor;
in vec3 fragCoord;

uniform sampler2D textureIn;

out vec4 fragColor;

void main() {
   fragColor = texture(textureIn, texCoord) * vertexColor;
}