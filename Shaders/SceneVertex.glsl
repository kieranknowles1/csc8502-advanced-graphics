#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec4 nodeColor;

in vec3 position;
in vec2 texCoord;

out Vertex {
    vec2 texCoord;
    vec4 color;
} OUT;

void main() {
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    OUT.texCoord = texCoord;
    OUT.color = nodeColor;
}