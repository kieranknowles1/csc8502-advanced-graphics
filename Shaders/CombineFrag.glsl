#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

#define AMBIENT_INTENSITY 0.1

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
    vec3 diffuse = texture(diffuseTex, IN.texCoord).rgb;
    vec3 light = texture(diffuseLight, IN.texCoord).rgb;
    vec3 specular = texture(specularLight, IN.texCoord).rgb;

    fragColor.rgb = diffuse * AMBIENT_INTENSITY;
    fragColor.rgb += diffuse * light;
    fragColor.rgb += specular;

    fragColor.a = 1.0;
}