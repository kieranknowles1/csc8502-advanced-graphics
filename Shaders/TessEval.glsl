#version 400

layout(quads, cw) in;

uniform sampler2D noiseTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec4 nodeColor;

#define NOISE_POWER 16.0f
#define NOISE_SCALE vec2(0.125f)

// From TessVertex.glsl
in Vertex {
    vec4 color;
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
#define DECLARE_MIXER(type, name) \
    type name(type a, type b, type c, type d) { \
        type p0 = mix(a, c, gl_TessCoord.x); \
        type p1 = mix(b, d, gl_TessCoord.x); \
        return mix(p0, p1, gl_TessCoord.y); \
    }
DECLARE_MIXER(vec2, quadMix2)
DECLARE_MIXER(vec3, quadMix3)
DECLARE_MIXER(vec4, quadMix4)

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

    OUT.color = quadMix4(
        IN[0].color,
        IN[1].color,
        IN[2].color,
        IN[3].color
    );
    OUT.worldPos = worldPos.xyz;

    // TES shaders have access to all patch vertices, so
    // we can use them to calculate the normal
    // NOTE: This doesn't account for the tessellated vertices,
    // but is close enough for my purposes
    // The CPP side only supports triangles, so we have to do this
    // manually
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    OUT.normal = normalize(cross(p1 - p0, p3 - p0));
    OUT.tangent = normalize(p1 - p0);
    OUT.binormal = normalize(p3 - p0);

#ifdef DEBUG
    OUT.color.rgb = vec3(gl_TessLevelOuter[0] / 16.0, 0, 0);
#endif
}
