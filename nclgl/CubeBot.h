#pragma once
#include "SceneNode.h"
class CubeBot : public SceneNode
{
public:
	CubeBot(std::shared_ptr<Mesh> cube);
	~CubeBot();

	const std::string getName() const override { return "CubeBot"; }

protected:
	CubeBot(const CubeBot& s);


	SceneNode* deepCopy() const override;
	void onUpdate(float dt) override;

	SceneNode* head;
	SceneNode* body;
	SceneNode* leftArm;
	SceneNode* rightArm;
	SceneNode* leftLeg;
	SceneNode* rightLeg;
};

