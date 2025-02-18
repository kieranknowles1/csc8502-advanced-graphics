#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D shadowTex;

uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAttenuation;

uniform float reflectionPower;

in Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 tangent;
    vec3 binormal;
    vec4 shadowProj;
} IN;

out vec4 fragColor;

// Low values look like plastic, high values look like metal
#define SHININESS 60
// The intensity of the specular highlight, higher is more reflective
#define SPECULAR_INTENSITY 0.33

#define AMBIENT 0.1

vec3 getBumpNormal() {
    mat3 TBN = mat3(
        normalize(IN.tangent),
        normalize(IN.binormal),
        normalize(IN.normal)
    );

    vec3 normal = texture(bumpTex, IN.texCoord).rgb;

    // We don't have a bump map, so use raw normal
    if (normal == vec3(0.0)) {
        return normalize(IN.normal);
    }

    // Transform the normal from the bump map to world space
    vec3 transformedNormal = TBN * normalize(normal * 2.0 - 1.0);

    return normalize(transformedNormal);
}

float getSpecularFactor(vec3 halfAngle) {
    vec3 bumpNormal = getBumpNormal();
    // bumpNormal = IN.normal;
    float clamped = clamp(dot(halfAngle, bumpNormal), 0.0, 1.0);
    return pow(clamped, SHININESS);
}

// Parameters:
// incident - the direction of the light
// coloredSurface - the color of the surface multiplied by the light color
// attenuation - The attenuation factor of the light at this point
vec3 getFinalDiffuse(vec3 incident, vec3 coloredSurface, float attenuation) {
    // How much light is hitting the surface?
    float lambert = max(dot(IN.normal, incident), 0.0);

    return coloredSurface * lambert * attenuation;
}

// Parameters:
// halfAngle - the midpoint between the incident and view vectors
// coloredSurface - the color of the surface multiplied by the light color
// attenuation - The attenuation factor of the light at this point
vec3 getFinalSpecular(vec3 halfAngle, vec3 coloredSurface, float attenuation) {
    return coloredSurface * getSpecularFactor(halfAngle) * attenuation * SPECULAR_INTENSITY;
    // return texture(cubeTex, halfAngle).rgb * getSpecularFactor(halfAngle) * attenuation * SPECULAR_INTENSITY;
}

vec3 getAmbient(vec3 coloredSurface) {
    return coloredSurface * AMBIENT;
}

vec4 getReflection(vec3 viewDir) {
    // TODO: Should we be reusuing getBumpNormal() here?
    // or will the driver optimise this? Don't know how to check
    vec3 reflectDir = reflect(-viewDir, normalize(getBumpNormal()));
    return texture(cubeTex, reflectDir) * reflectionPower;
}

float getAttenuation() {
    float distance = length(lightPos - IN.worldPos);
    if (distance > lightRadius) {
        return 0.0;
    }
    // This is a piecewise function, {0 < normalisedDistance < 1} -> {0 < attenuation < 1}
    float normalisedDistance = distance / lightRadius;

    float factorDistance = normalisedDistance * lightAttenuation;
    float factorDistanceSquared = pow(factorDistance, 2);

    // https://www.desmos.com/calculator/ecqboyzch2
    // http://learnwebgl.brown37.net/09_lights/lights_attenuation.html
    float attenuation = 1.0 / (1 + factorDistance + factorDistanceSquared);

    return clamp(attenuation, 0.0, 1.0);
}

float getShadow() {
    vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;

    // Are we in the shadow mapped region?
    bool maybeShaded = abs(shadowNDC.x) < 1.0f
        && abs(shadowNDC.y) < 1.0f
        && abs(shadowNDC.z) < 1.0f;

    if (maybeShaded) {
        vec3 biasCoord = shadowNDC * 0.5 + 0.5;
        // Texture always returns a vec4. This is a depth
        // texture, so we only need the first component
        float shadowDepth = texture(shadowTex, biasCoord.xy).x;
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
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 view = normalize(cameraPos - IN.worldPos);
    vec3 halfAngle = normalize(incident + view);
    vec3 reflectDir = reflect(-view, normalize(IN.normal));

    vec4 diffuse = texture(diffuseTex, IN.texCoord) * IN.color;
    vec3 bumpNormal = getBumpNormal();

    float attenuation = getAttenuation();

    vec3 coloredSurface = diffuse.rgb * lightColor;
    fragColor.rgb =
          getFinalDiffuse(incident, coloredSurface, attenuation)
        + getFinalSpecular(halfAngle, coloredSurface, attenuation)
        + getReflection(view).rgb;
    fragColor.rgb *= getShadow();

    // Ambient light always affects the surface
    fragColor.rgb += getAmbient(coloredSurface);
    fragColor.a = diffuse.a;
}
