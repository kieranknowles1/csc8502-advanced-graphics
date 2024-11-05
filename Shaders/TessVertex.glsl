#version 400 core
// Tessellation is a newer feature, and isn't required until OpenGL 4.0

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out Vertex {
    vec2 texCoord;
} OUT;

void main() {
    // Transformations must be done in tesseval otherwise
    // the tesselator will be working in screen space
    gl_Position = vec4(position, 1.0);
    OUT.texCoord = texCoord;
}