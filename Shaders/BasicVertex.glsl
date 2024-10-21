#version 330 core

// Input variables for position and color
in vec3 position;
in vec4 color;

// Output struct. Just a color for now
// Passed to the fragment shader
out Vertex {
    vec4 color;
} OUT;

void main() {
    // Set the magic gl_Position variable.
    // No transformations for now
    gl_Position = vec4(position, 1.0);
    OUT.color = color;
}