#version 400

layout(quads, cw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


// Interpolate vertex attributes based on abstract weightings from gl_TessCoord
#define DECLARE_MIXER(type, name) \
    type name(type a, type b, type c, type d) { \
        type p0 = mix(a, c, gl_TessCoord.x); \
        type p1 = mix(b, d, gl_TessCoord.x); \
        return mix(p0, p1, gl_TessCoord.y); \
    }
DECLARE_MIXER(vec3, quadMix3)

void main() {
    vec3 combinedPos = quadMix3(
        gl_in[0].gl_Position.xyz,
        gl_in[1].gl_Position.xyz,
        gl_in[2].gl_Position.xyz,
        gl_in[3].gl_Position.xyz
    );
    vec4 worldPos = modelMatrix * vec4(combinedPos, 1.0);

    gl_Position = projMatrix * viewMatrix * worldPos;
}
