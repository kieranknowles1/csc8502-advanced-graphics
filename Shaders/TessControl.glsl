#version 400 core
layout(vertices = 4) out; // From patches

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

// From TessVertex.glsl
in Vertex {
    vec4 color;
    vec2 texCoord;
} IN[];

// To TessEval.glsl
out Vertex {
    vec4 color;
    vec2 texCoord;
} OUT[];

// Level at MAX_LOD_DISTANCE
#define MIN_TESS_LEVEL 1
// Level at MIN_LOD_DISTANCE
#define MAX_TESS_LEVEL 4
// Distance at which MAX_TESS_LEVEL is used
#define MIN_LOD_DISTANCE 100.0
// Distance at which MIN_TESS_LEVEL is used
#define MAX_LOD_DISTANCE 2000.0

// Normalised distance from camera, 0.0 is at MIN_LOD_DISTANCE, 1.0 is at MAX_LOD_DISTANCE
float normalisedCameraDistance(vec3 pos) {
    const float LOD_RANGE = MAX_LOD_DISTANCE - MIN_LOD_DISTANCE;

    float distance = length((viewMatrix * vec4(pos, 1.0)).xyz);

    return clamp((distance - MIN_LOD_DISTANCE) / LOD_RANGE, 0.0, 1.0);
}

float tessLevel(float normalisedDistance) {
    // float ratio = pow(normalisedDistance, 0.5);
    float ratio = normalisedDistance;
    return mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, ratio);
}

// Set the tessellation levels based on the distance from the camera
// Based on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
void applyLevelOfDetail() {
    float level0 = tessLevel(normalisedCameraDistance(gl_in[0].gl_Position.xyz));
    float level1 = tessLevel(normalisedCameraDistance(gl_in[1].gl_Position.xyz));
    float level2 = tessLevel(normalisedCameraDistance(gl_in[2].gl_Position.xyz));
    float level3 = tessLevel(normalisedCameraDistance(gl_in[3].gl_Position.xyz));

    gl_TessLevelOuter[0] = level0;
    gl_TessLevelOuter[1] = level1;
    gl_TessLevelOuter[2] = level2;
    gl_TessLevelOuter[3] = level3;

    gl_TessLevelInner[0] = max(level1, level3);
    gl_TessLevelInner[1] = max(level0, level2);
}

void main() {
    // The first invocation is responsible for setting the tessellation levels
    if (gl_InvocationID == 0) {
        applyLevelOfDetail();
    }

    // Pass through the input values
    OUT[gl_InvocationID].color = IN[gl_InvocationID].color;
    OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;

    gl_out[gl_InvocationID].gl_Position =
        gl_in[gl_InvocationID].gl_Position;
}
