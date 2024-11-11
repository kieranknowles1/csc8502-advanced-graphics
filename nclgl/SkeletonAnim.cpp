#include "SkeletonAnim.h"

#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "ResourceManager.h"

SkeletonAnim::SkeletonAnim(ResourceManager* rm, std::string mesh, std::string anim, std::string mat)
{
	this->mesh = rm->getMeshes().get(mesh);
	// TODO: All of these could be managed resources
	this->anim = new MeshAnimation(anim);
	this->setMateriels(Materiel::fromFile(rm, mat));
	auto shader = rm->getShaders().get({ "SkinningVertex.glsl", "TexturedFragment.glsl" });
	for (auto& mat : materials) {
		mat.shader = shader;
	}
}

SkeletonAnim::~SkeletonAnim()
{
	delete anim;
}

void SkeletonAnim::drawSelf(OGLRenderer& r, bool shadowPass)
{
	// TODO: Support shadow pass
	if (shadowPass) return;

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

	Matrix4 model = getWorldTransform();

	for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
		materials[i].bind(r, r.getDefaultMateriel());
		r.UpdateShaderMatrices();
		model.bind(r.getCurrentShader()->getUniform("modelMatrix"));
		auto shader = materials[i].shader;
		glUniformMatrix4fv(
			shader->getUniform("joints"), frameMatrices.size(),
			false, (float*)frameMatrices.data()
		);
		mesh->DrawSubMesh(i);
	}
}

void SkeletonAnim::onUpdate(float dt)
{
	nextFrameTime -= dt;
	while (nextFrameTime < 0) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		nextFrameTime += 1.0f / anim->GetFrameRate();
	}
}
