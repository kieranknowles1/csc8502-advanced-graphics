#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

class Renderer : public OGLRenderer 
{
public:
	Renderer(Window &parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

	void setWaterLevel(float level) { waterLevel = level; }
	float getWaterLevel() const { return waterLevel; }

	float getWaveAmplitude() const { return waveAmplitude; }
	void setWaveAmplitude(float amplitude) { waveAmplitude = amplitude; }

protected:
	HeightMap* heightMap;
	Camera* camera;
	Shader* shader;
	GLuint terrainTexture;
	GLuint waterTexture;

	float time = 0.0f;
	float waveAmplitude = 0.01f;
	float waterLevel = 0.2;
};

