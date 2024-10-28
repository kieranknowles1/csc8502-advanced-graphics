#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];

out Vertex {
    vec2 texCoord;
} OUT;

vec4 weightedJointPos(vec4 localPos, int index) {
    // GLSL supports indexing vectors as if they were arrays
    mat4 joint = joints[jointIndices[index]];
    float weight = jointWeights[index];

    return joint * localPos * weight;
}

void main() {
    vec4 localPos = vec4(position, 1.0);
    vec4 skelPos = vec4(0, 0, 0, 0);

    for (int i = 0; i < 4; i++) {
        skelPos += weightedJointPos(localPos, i);
    }

    mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    gl_Position = mvp * skelPos;
    OUT.texCoord = texCoord;
}