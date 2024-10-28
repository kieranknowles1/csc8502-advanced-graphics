#include "SkeletonAnim.h"

#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "ResourceManager.h"

SkeletonAnim::SkeletonAnim(ResourceManager* rm, std::string mesh, std::string anim, std::string mat)
{
	// TODO: All of these could be managed resources
	this->mesh = Mesh::LoadFromMeshFile(mesh);
	this->anim = new MeshAnimation(anim);
	material = new MeshMaterial(mat);
	this->shader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");

	// TODO: We should throw on failure in LoadMeshFile and shader::shader
	if (!shader->LoadSuccess() || !this->mesh)
		throw std::exception("Failed to load shader or mesh");

	textures.reserve(this->mesh->GetSubMeshCount());
	for (int i = 0; i < this->mesh->GetSubMeshCount(); i++) {
		auto material = this->material->GetMaterialForLayer(i);
		const std::string* filename = nullptr;
		material->GetEntry("Diffuse", &filename);

		auto texture = rm->getTexture(
			// INVERT_Y is needed for Role_T.msh, flags
			// should be part of the material
			*filename, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
		);

		textures.emplace_back(texture);
	}
}

SkeletonAnim::~SkeletonAnim()
{
	delete mesh;
	delete anim;
	delete material;
	// TODO: We shouldn't be responsible for freeing our parent's resources
	delete shader;
}

void SkeletonAnim::drawSelf(OGLRenderer& r)
{
	auto prevShader = r.getCurrentShader();
	r.BindShader(shader);
	r.UpdateShaderMatrices();

	// TODO: Should this be part of the animation?
	std::vector<Matrix4> frameMatrices;
	frameMatrices.reserve(mesh->GetJointCount());
	auto currentFrameData = anim->GetJointData(currentFrame);
	auto nextFrameData = anim->GetJointData((currentFrame + 1) % anim->GetFrameCount());
	// TODO: Could we transform the mesh by this on load?
	auto inverseBindPose = mesh->GetInverseBindPose();
	float frameDuration = 1.0f / anim->GetFrameRate();
	float lerpFactor = 1.0f - (nextFrameTime / frameDuration);

	for (int i = 0; i < mesh->GetJointCount(); i++) {
		Matrix4 currentJoint = currentFrameData[i];
		Matrix4 nextJoint = nextFrameData[i];
		Matrix4 joint = Matrix4::lerp(currentJoint, nextJoint, lerpFactor);
		frameMatrices.emplace_back(joint * inverseBindPose[i]);
	}

	glUniformMatrix4fv(
		shader->getUniform("joints"), frameMatrices.size(),
		false, (float*)frameMatrices.data()
	);

	// TODO: Quite a bit of duplication with SceneNode::drawSelf
	auto model = getWorldTransform();
	model.bind(shader->getUniform("modelMatrix"));

	for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
		glActiveTexture(GL_TEXTURE0);
		textures[i]->bind();
		mesh->DrawSubMesh(i);
	}

	r.BindShader(prevShader);
}

void SkeletonAnim::onUpdate(float dt)
{
	nextFrameTime -= dt;
	while (nextFrameTime < 0) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		nextFrameTime += 1.0f / anim->GetFrameRate();
	}
}
