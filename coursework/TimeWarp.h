#pragma once

#include <glad/glad.h>

#include "../nclgl/PostProcess/Stage.h"
#include "../nclgl/ResourceManager.h"

class TimeWarp : public PostProcess::Stage {
public:
	TimeWarp(ResourceManager* rm, GLuint oldTex, GLuint newTe);
	~TimeWarp() override {}

	void setRatio(float ratio) {
		if (ratio < 0) ratio = 0;
		if (ratio > 1) ratio = 1;
		this->ratio = ratio;
	}
	float getRatio() const { return ratio; }

	void apply(OGLRenderer& r) override;

private:
	GLuint oldTex;
	GLuint newTex;
	std::shared_ptr<Shader> shader;

	float ratio;

	std::unique_ptr<Mesh> quad;
};