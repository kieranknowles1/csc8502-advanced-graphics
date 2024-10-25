#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float waterLevel;
// The amplitude of the wave. Very sensitive as
// it is a multiple of the total height of the mesh
uniform float waveAmplitude;

uniform float time;

in vec3 position;
in vec2 texCoord;
in vec4 color;

out Vertex {
    vec2 texCoord;
    vec4 color;
    float localWaterLevel;
} OUT;

void main() {
    OUT.localWaterLevel = waterLevel + (sin(
        time + (position.x + position.z) * 0.01
    ) * waveAmplitude);

    bool isWater = color.r < OUT.localWaterLevel;

    vec3 adjustedPosition = position;
    if (isWater) {
        // Heightmap ranges from 0-256, so we need to scale it
        adjustedPosition.y = OUT.localWaterLevel * 256;
    }

    mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    gl_Position = mvp * vec4(adjustedPosition, 1.0);
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

    OUT.color = color;
}