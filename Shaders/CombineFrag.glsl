#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D normalTex;
uniform sampler2D depthTex;
uniform sampler2D reflectivityTex;

uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform mat4 inverseProjView;

#define AMBIENT_INTENSITY 0.1
#define BRIGHTNESS 1.0

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

vec3 getWorldPosition(vec2 texCoord) {
    // This is effectively the inverse of the vertex shader's transformation
    float depth = texture(depthTex, texCoord).r;
    vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
    vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
    return invClipPos.xyz / invClipPos.w;
}

float sampleReflectivity(vec2 texCoord) {
    // Use only the red channel, the rest are reserved for future use
    return texture(reflectivityTex, texCoord).r;
}

vec4 reflectionColor(vec2 texCoord) {
    vec3 worldPos = getWorldPosition(texCoord);

    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 normal = texture(normalTex, texCoord).rgb * 2.0 - 1.0;

    vec3 reflectDir = reflect(viewDir, normal);

    return texture(cubeTex, reflectDir);
}

void main() {
    vec3 diffuse = texture(diffuseTex, IN.texCoord).rgb;
    vec3 light = texture(diffuseLight, IN.texCoord).rgb;
    vec3 specular = texture(specularLight, IN.texCoord).rgb;

    fragColor.rgb = diffuse * AMBIENT_INTENSITY;
    fragColor.rgb += diffuse * light * BRIGHTNESS;
    fragColor.rgb += specular;
    fragColor.rgb += reflectionColor(IN.texCoord).rgb * sampleReflectivity(IN.texCoord);

    fragColor.a = 1.0;
}