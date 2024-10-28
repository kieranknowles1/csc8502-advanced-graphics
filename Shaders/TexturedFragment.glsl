#version 330 core

// A sampler2D is a texture that can be sampled in the shader
uniform sampler2D diffuseTex;
in Vertex {
    vec2 texCoord;
    vec4 color;
} IN;

out vec4 fragColor;
void main() {
    fragColor = texture(diffuseTex, IN.texCoord) * IN.color;
}