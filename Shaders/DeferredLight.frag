#version 330 core

// Used for shadow mapping and projected textures
uniform mat4 shadowMatrix;

uniform sampler2D depthTex;
uniform sampler2D normalTex;

uniform bool useShadows;
uniform sampler2D shadowTex;

uniform bool useProjectedTexture;
uniform sampler2D projectedTexture;

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

struct ShadowData {
    vec3 normalisedDeviceCoords;
    // Are these coordinates within the bounds of the shadow map?
    bool inBounds;
};
ShadowData getShadowData(vec4 shadowProj) {
    vec3 ndc = shadowProj.xyz / shadowProj.w;
    bool inBounds = abs(ndc.x) < 1.0f
        && abs(ndc.y) < 1.0f
        && abs(ndc.z) < 1.0f;

    return ShadowData(ndc, inBounds);
}

float getShadow(vec4 shadowProj) {
    if (!useShadows) {
        return 1.0;
    }

    ShadowData shadowData = getShadowData(shadowProj);
    if (!shadowData.inBounds) {
        return 0.0; // We're outside the shadow map
    }

    vec3 biasCoord = shadowData.normalisedDeviceCoords * 0.5 + 0.5;
    // Texture always returns a vec4. This is a depth
    // texture, so we only need the first component
    float shadowDepth = texture(shadowTex, biasCoord.xy).x;
    // The light is further than what was recorded
    // in the shadow map, so we're in shadow
    // If the shadow depth is less than the bias, we must be behind an object
    return shadowDepth < biasCoord.z ? 0.0 : 1.0;
}

vec4 getProjectedTexture(vec4 shadowProj) {
    if (!useProjectedTexture) {
        return vec4(1.0);
    }

    ShadowData data = getShadowData(shadowProj);
    if (!data.inBounds) {
        return vec4(0.0); // Clamp to edge
    }

    return texture(projectedTexture, data.normalisedDeviceCoords.xy * 0.5 + 0.5);
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

    vec4 projectedTex = getProjectedTexture(shadowProj);

    vec3 attenuated = lightColor * attenuation * projectedTex.rgb;
    attenuated *= shadow;

    diffuseOutput = vec4(attenuated * lambert, 1.0);
    specularOutput = vec4(attenuated * specFactor * SPECULAR_INTENSITY, 1.0);
}
