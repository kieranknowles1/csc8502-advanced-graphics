#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;

in Vertex {
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main() {
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);

    vec3 reflectDir = reflect(-viewDir, normalize(IN.normal));
    vec4 reflectColour = texture(cubeTex, reflectDir);

    fragColour = reflectColour + (diffuse * 0.25);
}