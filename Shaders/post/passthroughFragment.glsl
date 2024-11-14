#version 330 core
// Fragment shader that does nothing other than sample the texture

uniform sampler2D sceneTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
    fragColor = texture(sceneTex, IN.texCoord);
}