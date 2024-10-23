#version 330 core

// A sampler2D is a texture that can be sampled in the shader
uniform sampler2D diffuseTex;
uniform sampler2D blendTex;

in Vertex {
    vec2 texCoord;
    vec4 color;
} IN;

out vec4 fragColor;
void main() {
    // We could swizzle to extract different colors
    // fragColor.brg = texture(diffuseTex, IN.texCoord).rgb;
    // fragColor = texture(diffuseTex, IN.texCoord) * texture(blendTex, IN.texCoord);

    fragColor = mix(
        texture(diffuseTex, IN.texCoord),
        texture(blendTex, IN.texCoord),
        clamp(IN.texCoord.y, 0, 1)
    );
}