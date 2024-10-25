#version 330 core

// A sampler2D is a texture that can be sampled in the shader
uniform sampler2D diffuseTex;
uniform sampler2D waterTex;

uniform float time;

// Discard any fragments with an alpha value below this threshold
// This emulates alpha testing as seen in older OpenGL versions
// Alpha testing is faster than translucency, but is a binary test
// Still, it's useful for certain effects. E.g., dense grass would
// be expensive to sort, but cheap to discard and small enough for
// the quality loss to be unnoticeable
uniform float alphaThreshold;

in Vertex {
    vec2 texCoord;
    vec4 color;
    float localWaterLevel;
} IN;

vec4 sample(bool isWater, vec2 texCoord) {
    // Move the texture coordinates over time to simulate water movement
    // Waves could also be generated using sin(time)
    return isWater
        ? texture(waterTex, vec2(texCoord.x + (time/16), texCoord.y))
        : texture(diffuseTex, texCoord);
}

out vec4 fragColor;
void main() {
    bool isWater = IN.color.r < IN.localWaterLevel;

    // NOTE: As color is used for shadows and determining water,
    // raising the water level will cause shadows to be cast on
    // the water surface. Keeping it in since I think it looks cool
    fragColor = sample(isWater, IN.texCoord) * IN.color;
}