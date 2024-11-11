#version 330 core

uniform mat4 shadowMatrix;

uniform sampler2D depthTex;
uniform sampler2D normalTex;

uniform bool useShadows;
uniform sampler2D shadowTex;

uniform vec2 pixelSize; // Reciprocal of the screen resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform float lightAttenuation;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightFOV;
// Forward vector of the light
uniform vec3 lightForward;

// The type of light
// Must match the enum in Light.h
const int Type_Point = 0;
const int Type_Sun = 1;
uniform int lightType;

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

bool isInsideFOV(vec3 incident) {
    return dot(incident, lightForward) > cos(radians(lightFOV));
}

float getAttenuation(vec3 worldPos) {
    if (lightType == Type_Sun) {
        return 1.0;
    }

    float distance = length(lightPos - worldPos);
    if (distance > lightRadius) {
        return 0.0;
    }

    // This is a piecewise function, {0 < normalisedDistance < 1} -> {0 < attenuation < 1}
    float normalisedDistance = distance / lightRadius;

    float factorDistance = normalisedDistance * lightAttenuation;
    float factorDistanceSquared = factorDistance * factorDistance;

    float attenuation = 1.0 / (1.0 + factorDistanceSquared);
    return clamp(attenuation, 0.0, 1.0);
}

float getShadow(vec4 shadowProj) {
    if (!useShadows) {
        return 1.0;
    }

    vec3 shadowNDC = shadowProj.xyz / shadowProj.w;

    // Are we in the shadow mapped region?
    bool maybeShaded = abs(shadowNDC.x) < 1.0f
        && abs(shadowNDC.y) < 1.0f
        && abs(shadowNDC.z) < 1.0f;

    if (maybeShaded) {
        vec3 biasCoord = shadowNDC * 0.5 + 0.5;
        // Texture always returns a vec4. This is a depth
        // texture, so we only need the first component
        float shadowDepth = texture(shadowTex, biasCoord.xy).x;
        // diffuseOutput = vec4(biasCoord, 1.0);
        // The light is further than what was recorded
        // in the shadow map, so we're in shadow
        if (shadowDepth < biasCoord.z) {
            return 0.0;
        }
        return 1.0;
    }

    // return 1.0;
    // We're outside the lit region
    return 0.0;
}

void main() {
    vec2 texCoord = gl_FragCoord.xy * pixelSize;
    vec3 worldPos = getWorldPosition(texCoord);
    float attenuation = getAttenuation(worldPos);
    if (attenuation <= 0.0) {
        discard;
    }

    vec3 incident = lightType == Type_Point ? normalize(lightPos - worldPos) : lightForward;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    if (!isInsideFOV(incident)) {
        discard;
    }

    vec3 normal = normalize(texture(normalTex, texCoord).xyz * 2.0 - 1.0);
    float lambert = clamp(dot(incident, normal), 0.0, 1.0);
    float specFactor = pow(
        clamp(dot(normal, halfDir), 0.0, 1.0),
        SPECULAR_EXPONENT
    );

    vec4 pushVal = vec4(normal, 0) * dot(incident, normal);
    vec4 shadowProj = shadowMatrix * (vec4(worldPos, 1.0) + pushVal);
    float shadow = getShadow(shadowProj);

    vec3 attenuated = lightColor * attenuation;
    attenuated *= shadow;

    diffuseOutput = vec4(attenuated * lambert, 1.0);
    specularOutput = vec4(attenuated * specFactor * SPECULAR_INTENSITY, 1.0);
}
