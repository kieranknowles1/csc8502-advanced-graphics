#version 330 core

#define FAR_CLIP 1000.0

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex {
    vec2 texCoord;
    vec4 color;
} IN;

out vec4 fragColor;

void main() {
    fragColor.rgba = IN.color.bgra;

    // https://gamedev.stackexchange.com/questions/93055/getting-the-real-fragment-depth-in-glsl
    // Fragment depth = gl_FragCoord.z / gl_FragCoord.w
    // Divide by FAR_CLIP to get a value between 0 and 1
    float depth = (gl_FragCoord.z / gl_FragCoord.w) / FAR_CLIP;
    fragColor.rgb = vec3(depth, depth, depth);

    if (useTexture != 0) {
        fragColor *= texture(diffuseTex, IN.texCoord);
    }
}