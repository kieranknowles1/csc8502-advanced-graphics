#version 400 core
layout(vertices = 4) out; // From patches

// From TessVertex.glsl
// We don't care about any of this, this is just to reduce duplication
in Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec4 tangent;
} IN[];

void main() {
    // The first invocation is responsible for setting the tessellation levels
    if (gl_InvocationID == 0) {
        // Don't tesselate shadows, using more than 1 level would be overkill
        // Shadows are tolerant enough that low poly is fine
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = 1;
        gl_TessLevelOuter[2] = 1;
        gl_TessLevelOuter[3] = 1;

        gl_TessLevelInner[0] = 1;
        gl_TessLevelInner[1] = 1;
    }
    gl_out[gl_InvocationID].gl_Position =
        gl_in[gl_InvocationID].gl_Position;
}
