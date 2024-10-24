#pragma once
#include "SceneNode.h"
class CubeBot : public SceneNode
{
public:
	CubeBot(Mesh* cube);
	~CubeBot();

protected:
	void onUpdate(float dt) override;

	SceneNode* head;
	SceneNode* body;
	SceneNode* leftArm;
	SceneNode* rightArm;
	SceneNode* leftLeg;
	SceneNode* rightLeg;
};

