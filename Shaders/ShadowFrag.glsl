#version 330 core

out vec4 fragColor;

void main() {
    // Color is disabled, so this will just write depth
    fragColor = vec4(1.0);
}
