#pragma once

#include <vector>
#include <memory>

#include "SceneNode.h"

class MeshAnimation;
class MeshMaterial;
class ResourceManager;
class ManagedTexture;

class SkeletonAnim : public SceneNode
{
public:
	SkeletonAnim(ResourceManager* rm, std::string mesh, std::string anim, std::string mat);
	~SkeletonAnim() override;

	const std::string getName() const override { return "SkeletonAnim"; }
	void drawSelf(OGLRenderer& r) override;
protected:
	void onUpdate(float dt) override;
	SceneNode* deepCopy() const override { throw std::runtime_error("Not implemented"); }

	MeshAnimation* anim;
	MeshMaterial* material;
	std::vector<std::shared_ptr<ManagedTexture>> textures;

	int currentFrame = 0;
	float nextFrameTime = 0.0f;
};

