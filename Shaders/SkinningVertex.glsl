#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec4 nodeColor;

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec4 tangent;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];

out Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} OUT;

vec4 weightedJointPos(vec4 localPos, int index) {
    // GLSL supports indexing vectors as if they were arrays
    mat4 joint = joints[jointIndices[index]];
    float weight = jointWeights[index];

    return joint * localPos * weight;
}

void main() {
    OUT.color = color * nodeColor;
    OUT.texCoord = texCoord;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    OUT.normal = normalize(normalMatrix * normal);
    OUT.tangent = normalize(normalMatrix * tangent.xyz);
    OUT.binormal = cross(OUT.normal, OUT.tangent) * tangent.w;

    // Our local position in model space
    vec4 localPos = vec4(position, 1.0);
    // Our skinned position transformed according to the animation
    vec4 skelPos = vec4(0, 0, 0, 0);

    for (int i = 0; i < 4; i++) {
        skelPos += weightedJointPos(localPos, i);
    }

    vec4 worldPos = modelMatrix * skelPos;
    OUT.worldPos = worldPos.xyz;
    gl_Position = projMatrix * viewMatrix * worldPos;

    // mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    // gl_Position = mvp * skelPos;
    // OUT.texCoord = texCoord;
}