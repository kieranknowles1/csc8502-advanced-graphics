#version 330 core

// Simple gaussian blur shader
// Each pass takes the weighted sum of nearby pixels, with weights
// decreasing as distance increases
// For performance, we reduce this to two 1D blurs, alternating between
// horizontal and vertical with each pass

uniform sampler2D sceneTex;
uniform bool isVertical;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

// The scale factors for the gaussian kernel. Must sum to 1
const float ScaleFactors[7] = float[](
    0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
);

// Get the size of a texel for the current axis
vec2 getDelta() {
    return isVertical ? dFdy(IN.texCoord) : dFdx(IN.texCoord);
}

void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    // Work out how much 1 pixel is in texture space
    vec2 delta = getDelta();

    for (int i = 0; i < 7; i++) {
        vec2 offset = delta * (i - 3);
        vec4 sample = texture(sceneTex, IN.texCoord + offset);
        fragColor += sample * ScaleFactors[i];
    }
}