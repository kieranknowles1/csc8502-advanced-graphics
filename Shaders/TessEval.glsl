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
    vec3 normal;
    vec4 tangent;
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

    vec3 thisNormal = quadMix3(
        IN[0].normal,
        IN[1].normal,
        IN[2].normal,
        IN[3].normal
    );
    vec4 thisTangent = quadMix4(
        IN[0].tangent,
        IN[1].tangent,
        IN[2].tangent,
        IN[3].tangent
    );

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    OUT.normal = normalize(normalMatrix * thisNormal);

    vec3 worldNormal = normalize(normalMatrix * normalize(thisNormal));
    vec3 worldTangent = normalize(normalMatrix * normalize(thisTangent.xyz));
    OUT.tangent = worldTangent;
    OUT.binormal = cross(worldNormal, worldTangent) * thisTangent.w;

#ifdef DEBUG
    OUT.color.rgb = vec3(gl_TessLevelOuter[0] / 16.0, 0, 0);
#endif
}
