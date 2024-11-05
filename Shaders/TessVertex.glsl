#version 400 core
// Tessellation is a newer feature, and isn't required until OpenGL 4.0

uniform vec4 nodeColor;

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec4 tangent;

out Vertex {
    vec4 color;
    vec2 texCoord;
} OUT;

void main() {
    // Transformations must be done in tesseval otherwise
    // the tesselator will be working in screen space
    gl_Position = vec4(position, 1.0);
    OUT.color = color * nodeColor;
    OUT.texCoord = texCoord;
}