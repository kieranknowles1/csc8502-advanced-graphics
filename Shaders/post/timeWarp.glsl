#version 330 core

uniform sampler2D oldTex;
uniform sampler2D newTex;
uniform float ratio;
// const float ratio = 0.5;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

float distortionPower(float visibility) {
    return visibility / 16;
}

vec2 distort(vec2 coord, float power) {
    // Use a sine wave to distort different parts of the screen differently
    coord.x += sin(coord.y * 32) * power;
    coord.y += cos(coord.x * 32) * power;

    return coord;
}

void main() {
    // Distort sample coordinates based on how far they are from full visibility
    vec2 oldCoord = distort(IN.texCoord, distortionPower(ratio));
    vec4 oldSample = texture(oldTex, oldCoord);

    vec2 newCoord = distort(IN.texCoord, distortionPower(1 - ratio));
    vec4 newSample = texture(newTex, newCoord);
    // newSample.r += tan(newCoord.x * 32) * (1 - ratio) * 0.05 * sin(newCoord.y * 32);

    // Mix the two samples based on the ratio    
    fragColor = mix(oldSample, newSample, ratio);
}