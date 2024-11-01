#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normalTex;

uniform vec2 pixelSize; // Reciprocal of the screen resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform mat4 inverseProjView;

#define SPECULAR_EXPONENT 60.0
#define SPECULAR_INTENSITY 0.33

out vec4 diffuseOutput;
out vec4 specularOutput;

vec3 getWorldPosition(vec2 texCoord) {
    // This is effectively the inverse of the vertex shader's transformation
    float depth = texture(depthTex, texCoord).r;
    vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
    vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
    return invClipPos.xyz / invClipPos.w;
}

void main() {

    vec2 texCoord = gl_FragCoord.xy * pixelSize;
    vec3 worldPos = getWorldPosition(texCoord);

    float distance = length(worldPos - lightPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    if (attenuation <= 0.0) {
        discard;
    }

    vec3 normal = normalize(texture(normalTex, texCoord).xyz * 2.0 - 1.0);
    vec3 incident = normalize(lightPos - worldPos);
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float lambert = clamp(dot(incident, normal), 0.0, 1.0);
    float specFactor = pow(
        clamp(dot(normal, halfDir), 0.0, 1.0),
        SPECULAR_EXPONENT
    );

    vec3 attenuated = lightColor * attenuation;

    diffuseOutput = vec4(attenuated * lambert, 1.0);
    specularOutput = vec4(attenuated * specFactor * SPECULAR_INTENSITY, 1.0);
}