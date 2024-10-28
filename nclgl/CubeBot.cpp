#include "CubeBot.h"

CubeBot::CubeBot(Mesh* cube) {
	const Vector4 RED = Vector4(1, 0, 0, 1);
	const Vector4 GREEN = Vector4(0, 1, 0, 1);
	const Vector4 BLUE = Vector4(0, 0, 1, 1);
	const Vector4 YELLOW = Vector4(1, 1, 0, 1);

	// Bounding sphere is based on the bottom of the mesh, so we need to
	// use 2sqrt(2) to approximately cover the whole mesh
	float radius = 2.0f * sqrt(2);

	body = new SceneNode(cube, RED);
	body->setScale(Vector3(10, 15, 5));
	body->setTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	body->setBoundingRadius(radius);
	addChild(body);

	head = new SceneNode(cube, GREEN);
	head->setScale(Vector3(5, 5, 5));
	head->setTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	head->setBoundingRadius(radius);
	addChild(head);

	leftArm = new SceneNode(cube, BLUE);
	leftArm->setScale(Vector3(3, -18, 3));
	leftArm->setTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	leftArm->setBoundingRadius(radius);
	addChild(leftArm);

	rightArm = new SceneNode(cube, BLUE);
	rightArm->setScale(Vector3(3, -18, 3));
	rightArm->setTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	rightArm->setBoundingRadius(radius);
	addChild(rightArm);

	leftLeg = new SceneNode(cube, YELLOW);
	leftLeg->setScale(Vector3(3, -17.5, 3));
	leftLeg->setTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	leftLeg->setBoundingRadius(radius);
	addChild(leftLeg);

	rightLeg = new SceneNode(cube, YELLOW);
	rightLeg->setScale(Vector3(3, -17.5, 3));
	rightLeg->setTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	rightLeg->setBoundingRadius(radius);
	addChild(rightLeg);
}

CubeBot::~CubeBot()
{
}

void CubeBot::onUpdate(float dt)
{
	return;
	transform = transform
		* Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	leftArm->setTransform(leftArm->getTransform()
		* Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));

	rightArm->setTransform(rightArm->getTransform()
		* Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

}

CubeBot::CubeBot(const CubeBot& s)
	: SceneNode(s)
	, body(s.body->deepCopy())
	, head(s.head->deepCopy())
	, leftArm(s.leftArm->deepCopy())
	, rightArm(s.rightArm->deepCopy())
	, leftLeg(s.leftLeg->deepCopy())
	, rightLeg(s.rightLeg->deepCopy())
{
	addChild(body);
	addChild(head);
	addChild(leftArm);
	addChild(rightArm);
	addChild(leftLeg);
	addChild(rightLeg);
}

SceneNode* CubeBot::deepCopy() const
{
	return new CubeBot(*this);
}