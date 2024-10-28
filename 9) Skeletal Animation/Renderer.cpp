#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, camera(new Camera())
	, shader(new Shader("SkinningVertex.glsl", "TexturedFragment.glsl"))
	, mesh(Mesh::LoadFromMeshFile("Role_T.msh"))
	, anim(new MeshAnimation("Role_T.anm"))
	, material(new MeshMaterial("Role_T.mat"))
	, currentFrame(0)
	, frameTime(0.0f)
{
	if (!shader->LoadSuccess() || !mesh) return;
	camera->setSpeed(10.0f);

	projMatrix = Matrix4::Perspective(
		1.0f, 10000.0f,
		(float)width / (float)height,
		45.0f
	);

	bool badTexture = false;
	for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
		auto entry = material->GetMaterialForLayer(i);
		const std::string* filename = nullptr;
		entry->GetEntry("Diffuse", &filename);

		auto path = TEXTUREDIR + *filename;
		GLuint id = SOIL_load_OGL_texture(
			path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
		);
		badTexture |= id == 0;
		matTextures.emplace_back(id);
	}
	if (badTexture) return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer()
{
	delete camera;
	delete mesh;
	delete shader;
	delete anim;
	delete material;

	glDeleteTextures(matTextures.size(), matTextures.data());
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();

	frameTime -= dt;
	while (frameTime < 0) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0 / anim->GetFrameRate();
	}
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	glUniform1i(shader->getUniform("diffuseTex"), 0);
	UpdateShaderMatrices();

	// Remove the transformation of the bind pose from vertexes
	// by multiplying the current position of each joint by the
	// inverse of the bind pose
	std::vector<Matrix4> frameMatrices;
	frameMatrices.reserve(mesh->GetJointCount());

	const Matrix4* currentFrameData = anim->GetJointData(currentFrame);
	const Matrix4* inverseBindPose = mesh->GetInverseBindPose();

	for (int i = 0; i < mesh->GetJointCount(); i++) {
		frameMatrices.emplace_back(currentFrameData[i] * inverseBindPose[i]);
	}

	glUniformMatrix4fv(
		shader->getUniform("joints"), frameMatrices.size(),
		false, (float*)frameMatrices.data()
	);

	// Draw each sub-mesh
	for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}