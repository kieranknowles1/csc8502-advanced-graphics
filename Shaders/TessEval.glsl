#version 400

layout(quads, cw) in;

uniform sampler2D noiseTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec4 nodeColor;

#define NOISE_POWER 32.0f
#define NOISE_SCALE vec2(0.25f)

// From TessVertex.glsl
in Vertex {
    vec2 texCoord;
} IN[];

// To BufferFragment.glsl
out Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} OUT; // Not an array, this runs for each tessellated vertex

// Interpolate vertex attributes based on abstract weightings from gl_TessCoord
vec3 quadMix3(vec3 a, vec3 b, vec3 c, vec3 d) {
    vec3 p0 = mix(a, c, gl_TessCoord.x);
    vec3 p1 = mix(b, d, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

vec2 quadMix2(vec2 a, vec2 b, vec2 c, vec2 d) {
    vec2 p0 = mix(a, c, gl_TessCoord.x);
    vec2 p1 = mix(b, d, gl_TessCoord.x);
    return mix(p0, p1, gl_TessCoord.y);
}

void main() {
    vec3 combinedPos = quadMix3(
        gl_in[0].gl_Position.xyz,
        gl_in[1].gl_Position.xyz,
        gl_in[2].gl_Position.xyz,
        gl_in[3].gl_Position.xyz
    );

    vec2 texCoord = quadMix2(
        IN[0].texCoord,
        IN[1].texCoord,
        IN[2].texCoord,
        IN[3].texCoord
    );
    OUT.texCoord = (vec4(texCoord, 0.0, 1.0)).xy;

    vec4 worldPos = modelMatrix * vec4(combinedPos, 1.0);
    float noise = texture(noiseTex, OUT.texCoord * NOISE_SCALE).r;
    worldPos.y += noise * NOISE_POWER;

    gl_Position = projMatrix * viewMatrix * worldPos;

    // TODO: Set these properly
    OUT.color = vec4(1.0);
    OUT.normal = vec3(0.0, 1.0, 0.0);
    OUT.tangent = vec3(1.0, 0.0, 0.0);
    OUT.binormal = vec3(0.0, 0.0, 1.0);
    OUT.worldPos = worldPos.xyz;
}