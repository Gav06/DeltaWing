#version 460 core

in vec2 texCoord;
in vec3 fragCoord;

uniform sampler2D textureIn;

out vec4 fragColor;

void main() {
   vec4 texelColor = texture(textureIn, texCoord);
   // fragColor = texelColor;
   fragColor = vec4(texCoord, 0.0, 1.0);
}