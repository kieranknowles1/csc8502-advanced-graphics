#version 330 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex {
    vec2 texCoord;
    vec4 color;
} IN;

out vec4 fragColor;

void main() {
    fragColor = IN.color;
    if (useTexture != 0) {
        fragColor *= texture(diffuseTex, IN.texCoord);
    }
}