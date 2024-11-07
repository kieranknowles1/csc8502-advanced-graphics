#version 330 core

uniform sampler2D oldTex;
uniform sampler2D newTex;
uniform float ratio;
// const float ratio = 0.5;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

// Generate a triangle wave
float triangle(float x) {
    return abs(fract(x + 0.5) * 2 - 1);
}

float sawtooth(float x) {
    return fract(x);
}

float distortionPower(float visibility) {
    return visibility / 16;
}

vec2 distort(vec2 coord, float power) {
    // Use a sine wave to distort different parts of the screen differently
    // Each layer takes in the previous distortion as input, combining them to make a more complex distortion
    coord.x += sin(coord.y * 32) * power;
    coord.y += cos(coord.x * 32) * power;
    // Add a triangle wave component, which creates the effect of
    // a jagged edge
    coord.x += triangle(coord.y * 32) * power * 4;
    // coord.x += sawtooth(coord.y * 32) * power * 4; 
    // GL_CLAMP is set to GL_MIRRORED_REPEAT, so we don't need to clamp here

    return coord;
}

void main() {
    // Distort sample coordinates based on how far they are from full visibility
    // Combined with the mixing, this creates the effect of one image fading into another
    vec2 oldCoord = distort(IN.texCoord, distortionPower(ratio));
    vec4 oldSample = texture(oldTex, oldCoord);

    // Ratio is between 0 and 1, so 1 - ratio means the patterns match at the midpoint
    vec2 newCoord = distort(IN.texCoord, distortionPower(1 - ratio));
    vec4 newSample = texture(newTex, newCoord);
    // newSample.r += tan(newCoord.x * 32) * (1 - ratio) * 0.05 * sin(newCoord.y * 32);

    // Mix the two samples based on the ratio    
    fragColor = mix(oldSample, newSample, ratio);
}