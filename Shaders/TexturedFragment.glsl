#version 330 core

// A sampler2D is a texture that can be sampled in the shader
uniform sampler2D diffuseTex;

// Discard any fragments with an alpha value below this threshold
// This emulates alpha testing as seen in older OpenGL versions
// Alpha testing is faster than translucency, but is a binary test
// Still, it's useful for certain effects. E.g., dense grass would
// be expensive to sort, but cheap to discard and small enough for
// the quality loss to be unnoticeable
uniform float alphaThreshold;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;
void main() {
    // We could swizzle to extract different colors
    // fragColor.brg = texture(diffuseTex, IN.texCoord).rgb;
    // fragColor = texture(diffuseTex, IN.texCoord) * texture(blendTex, IN.texCoord);
    // fragColor = texture(diffuseTex, vec2(0.5, 0.5));
    fragColor = texture(diffuseTex, IN.texCoord);
}