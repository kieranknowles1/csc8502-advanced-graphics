#version 330 core

// Uniforms are constant for all vertices in the draw call
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Input variables for position and color
in vec3 position;
in vec4 color;

// Output struct. Just a color for now
// Passed to the fragment shader
out Vertex {
    vec4 color;
} OUT;

void main() {
    // Multiply all matrixes to get the "model view projection" matrix
    // "viewMatrix" is unused and kept as the identity matrix
    mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    
    // Transform the position by the MVP matrix
    gl_Position = mvp * vec4(position, 1.0);
    OUT.color = color;
}