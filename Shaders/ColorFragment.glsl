#version 330 core

// Take in the output from the vertex shader
in Vertex {
    vec4 color;
} IN;

out vec4 fragColor;

// Simple pass-through fragment shader
void main() {
    fragColor = IN.color;
}