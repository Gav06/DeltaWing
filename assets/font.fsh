#version 460 core

in vec2 texCoord;

uniform sampler2D textureIn;
uniform vec4 colorIn;

out vec4 fragColor;

void main() {
    vec4 texelColor = texture(textureIn, texCoord);
    fragColor = texelColor * colorIn;
}