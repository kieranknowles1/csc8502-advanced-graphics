#version 330 core
// Vertex shader that does nothing other than pass through position
// and texture coordinates to the fragment shader
in vec3 position;
in vec2 texCoord;

out Vertex {
    vec2 texCoord;
} OUT;

void main() {
    gl_Position = vec4(position, 1.0);
    OUT.texCoord = texCoord;
}