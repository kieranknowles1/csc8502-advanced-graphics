#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

// If true, blend colors based on alpha channel
// Otherwise, render as opaque with alpha test
uniform bool useBlending;

#define ALPHA_TEST_THRESHOLD 0.5

in Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4 fragColor[2];

void main() {
    mat3 TBN = mat3(
        normalize(IN.tangent),
        normalize(IN.binormal),
        normalize(IN.normal)
    );

    vec4 diffuse = texture2D(diffuseTex, IN.texCoord) * IN.color;
    if (!useBlending) {
        if (diffuse.a < ALPHA_TEST_THRESHOLD) {
            discard; 
        }
        diffuse.a = 1.0;
    }
    vec3 normal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
    normal = normalize(TBN * normal);

    fragColor[0] = diffuse;
    fragColor[1] = vec4(normal * 0.5 + 0.5, 1.0);
}
