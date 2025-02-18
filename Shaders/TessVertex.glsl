#version 400 core
// Tessellation is a newer feature, and isn't required until OpenGL 4.0

uniform mat4 modelMatrix;
uniform vec4 nodeColor;

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec4 tangent;

out Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec4 tangent;
} OUT;

void main() {
    // Transformations must be done in tesseval otherwise
    // the tesselator will be working in screen space
    gl_Position = modelMatrix * vec4(position, 1.0);
    OUT.color = color * nodeColor;
    OUT.texCoord = texCoord;

    OUT.normal = normal;
    OUT.tangent = tangent;
}