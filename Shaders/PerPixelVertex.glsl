#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texCoord;

out Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} OUT;

void main() {
    OUT.color = color;
    OUT.texCoord = texCoord;

    // Transform the normal by the inverse transpose of the model matrix
    // we need this to avoid distortion after uneven scaling or translation
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    OUT.normal = normalize(normalMatrix * normal);

    // Pass the world position to the fragment shader, used for lighting calculations
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    OUT.worldPos = worldPos.xyz;

    // Since we already need the world position in the fragment shader, we can
    // reuse it instead of multiplying by modelMatrix again
    gl_Position = projMatrix * viewMatrix * worldPos;
}