#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
    vec3 color;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColor;

// Low values look like plastic, high values look like metal
#define SHININESS 60
// The intensity of the specular highlight, higher is more reflective
#define SPECULAR_INTENSITY 0.33

#define AMBIENT 0.1

float getSpecularFactor(vec3 halfAngle) {
    float clamped = clamp(dot(halfAngle, IN.normal), 0.0, 1.0);
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
}

vec3 getAmbient(vec3 coloredSurface) {
    return coloredSurface * AMBIENT;
}

void main() {
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 view = normalize(cameraPos - IN.worldPos);
    vec3 halfAngle = normalize(incident + view);

    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    vec3 coloredSurface = diffuse.rgb * lightColor.rgb;
    fragColor.rgb =
          getFinalDiffuse(incident, coloredSurface, attenuation)
        + getFinalSpecular(halfAngle, coloredSurface, attenuation)
        + getAmbient(coloredSurface);
    fragColor.a = diffuse.a;
}