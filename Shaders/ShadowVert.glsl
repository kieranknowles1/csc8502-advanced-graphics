#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

// Simple shader that just applies transforms to the vertex position
void main() {
    gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}