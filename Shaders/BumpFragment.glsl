#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAttenuation;

in Vertex {
    vec4 color;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 tangent;
    vec3 binormal;
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
        + getAmbient(coloredSurface);
    fragColor.a = diffuse.a;
}
