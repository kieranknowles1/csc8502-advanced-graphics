#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

uniform vec3 lightPos;

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texCoord;
in vec4 tangent;

out Vertex {
    vec3 color;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 tangent;
    vec3 binormal;
    vec4 shadowProj;
} OUT;

void main() {
    OUT.color = color;
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

    // Transform the normal by the inverse transpose of the model matrix
    // we need this to avoid distortion after uneven scaling or translation
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    OUT.normal = normalize(normalMatrix * normalize(normal));
    OUT.tangent = normalize(normalMatrix * normalize(tangent.xyz));
    OUT.binormal = cross(OUT.normal, OUT.tangent) * tangent.w;

    // Pass the world position to the fragment shader, used for lighting calculations
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    OUT.worldPos = worldPos.xyz;


    vec3 viewDir = normalize(lightPos - worldPos.xyz);
    // Bias vertexes facing the light to avoid acne
    // This works by pushing the vertex along its normal towards the light
    vec4 pushVal = vec4(OUT.normal, 0) * dot(viewDir, OUT.normal);
    OUT.shadowProj = shadowMatrix * (worldPos + pushVal);

    // Since we already need the world position in the fragment shader, we can
    // reuse it instead of multiplying by modelMatrix again
    gl_Position = projMatrix * viewMatrix * worldPos;
}