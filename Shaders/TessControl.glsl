#version 400 core
layout(vertices = 4) out; // From patches

// From TessVertex.glsl
in Vertex {
    vec4 color;
    vec2 texCoord;
} IN[];

// To TessEval.glsl
out Vertex {
    vec4 color;
    vec2 texCoord;
} OUT[];

// NOTE: The number of vertices is TESS_LEVEL^2
#define TESS_LEVEL 4

void main() {
    // TODO: Set dynamically based on distance from camera
    gl_TessLevelInner[0] = TESS_LEVEL;
    gl_TessLevelInner[1] = TESS_LEVEL;
    gl_TessLevelOuter[0] = TESS_LEVEL;
    gl_TessLevelOuter[1] = TESS_LEVEL;
    gl_TessLevelOuter[2] = TESS_LEVEL;
    gl_TessLevelOuter[3] = TESS_LEVEL;

    // Pass through the input values
    OUT[gl_InvocationID].color = IN[gl_InvocationID].color;
    OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;

    gl_out[gl_InvocationID].gl_Position =
        gl_in[gl_InvocationID].gl_Position;
}