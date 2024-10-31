#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())

	, camera(std::make_unique<Camera>(parent.GetKeyboard(), parent.GetMouse(), -30, 315, 0, Vector3(-8, 5, 8)))
	, light(std::make_unique<Light>(Vector3(-20, 10, -20), Vector3(1, 1, 1), 250.0f))

	, sceneShader(std::make_unique<Shader>("ShadowSceneVert.glsl", "ShadowSceneFrag.glsl"))
	, shadowShader(std::make_unique<Shader>("ShadowVert.glsl", "ShadowFrag.glsl"))

	, sceneDiffuse(resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}))
	, sceneBump(resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}))

	, sceneTime(0)
{
	camera->setSpeed(10.0f);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowSize, ShadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE); // Don't draw anything to the colour buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	meshes.emplace_back(BasicNode(
		Mesh::GenerateQuad(),
		Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(10, 10, 1))
	));
	meshes.emplace_back(BasicNode(Mesh::LoadFromMeshFile("Sphere.msh")));
	meshes.emplace_back(BasicNode(Mesh::LoadFromMeshFile("Cylinder.msh")));
	meshes.emplace_back(BasicNode(Mesh::LoadFromMeshFile("Cone.msh")));

	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::UpdateScene(float dt) {
	camera->update(dt);
	sceneTime += dt;

	// Skip the floor
	for (int i = 1; i < meshes.size(); i++) {
		Vector3 t = Vector3(-10 + (i * 5), 2 + sin(sceneTime * i), 0);
		meshes[i].transform = Matrix4::Translation(t) *
			Matrix4::Rotation(sceneTime * 10 * i, Vector3(0, 1, 0));
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawShadowScene();
	drawMainScene();
}

void Renderer::drawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	// Temporarily increase OpenGL's viewport size so that we can render at higher than screen resolution
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, ShadowSize, ShadowSize);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // No color buffer is available

	BindShader(shadowShader.get());

	// Create a view matrix for the light
	viewMatrix = Matrix4::BuildViewMatrix(light->getPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45); // 45 degree field of view, aspect ratio 1:1, near plane at 1, far plane at 100
	shadowMatrix = projMatrix * viewMatrix;

	for (auto& mesh : meshes) {
		modelMatrix = mesh.transform;
		UpdateShaderMatrices();
		mesh.mesh->Draw();
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Restore the color buffer
	glViewport(0, 0, width, height); // Restore the viewport to the screen size
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the shadow FBO
}

void Renderer::drawMainScene() {
	BindShader(sceneShader.get());
	light->bind(*this);
	viewMatrix = camera->buildViewMatrix();
	projMatrix = Matrix4::Perspective(1, 150000, (float)width / (float)height, 45);

	glUniform1i(sceneShader->getUniform("diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	sceneDiffuse->bind();

	glUniform1i(sceneShader->getUniform("bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	sceneBump->bind();

	glUniform1i(sceneShader->getUniform("shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3f(sceneShader->getUniform("cameraPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	for (auto& mesh : meshes) {
		modelMatrix = mesh.transform;
		UpdateShaderMatrices();
		mesh.mesh->Draw();
	}
}
