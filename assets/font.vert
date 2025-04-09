#version 460 core

// usual vertex stuff
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
// instance data
layout (location = 2) in vec2 iPos;         // instance pos
layout (location = 3) in vec2 iSize;        // quad width & height
layout (location = 4) in vec2 iUv;          // instance UV (top left)
layout (location = 5) in vec2 iUvSize;      // instance UV width & height (advance from top left)

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    vec2 transformedPos = iPos + (aPos.xy * iSize);

    gl_Position = projection * model * vec4(transformedPos, aPos.z, 1.0);
    
    texCoord = iUv + (aTex * iUvSize);
}