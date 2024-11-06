#include "TimeWarp.h"

TimeWarp::TimeWarp(ResourceManager* rm, GLuint oldTex, GLuint newTex)
	: oldTex(oldTex)
	, newTex(newTex)
	, ratio(0.0f)
	, shader(rm->getShaders().get({ "post/passthroughVertex.glsl", "post/timeWarp.glsl" }))
	, quad(Mesh::GenerateQuad())
{}

void TimeWarp::apply(OGLRenderer& r) {
	r.BindShader(shader.get());

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shader->getUniform("oldTex"), 0);
	glBindTexture(GL_TEXTURE_2D, oldTex);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(shader->getUniform("newTex"), 1);
	glBindTexture(GL_TEXTURE_2D, newTex);

	glUniform1f(shader->getUniform("ratio"), ratio);

	quad->Draw();
}