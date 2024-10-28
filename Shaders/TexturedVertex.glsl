#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 color;

out Vertex {
    vec2 texCoord;
} OUT;

void main() {
    mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    gl_Position = mvp * vec4(position, 1.0);
    OUT.texCoord = texCoord;
}