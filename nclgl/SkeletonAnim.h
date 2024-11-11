#pragma once

// TODO: This needs complete reworking

#include <vector>
#include <memory>

#include "SceneNode.h"

class MeshAnimation;
class MeshMaterial;
class ResourceManager;
class Texture;

class SkeletonAnim : public SceneNode
{
public:
	SkeletonAnim(ResourceManager* rm, std::string mesh, std::string anim, std::string mat);
	~SkeletonAnim() override;

	const std::string getName() const override { return "SkeletonAnim"; }
	void drawSelf(OGLRenderer& r, bool shadowPass) override;
protected:
	void onUpdate(float dt) override;

	SkeletonAnim(const SkeletonAnim& o)
		: SceneNode(o)
		, anim(o.anim)
		, currentFrame(o.currentFrame)
		, nextFrameTime(o.nextFrameTime)
	{

	}
	SceneNode* deepCopy() const override {
		auto n = new SkeletonAnim(*this);
		n->copyChildrenFrom(*this);
		return n;
	}

	std::shared_ptr<MeshAnimation> anim;

	int currentFrame = 0;
	float nextFrameTime = 0.0f;
};

