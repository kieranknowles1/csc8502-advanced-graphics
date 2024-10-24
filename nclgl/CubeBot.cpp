#include "CubeBot.h"

CubeBot::CubeBot(Mesh* cube) {
	const Vector4 RED = Vector4(1, 0, 0, 1);
	const Vector4 GREEN = Vector4(0, 1, 0, 1);
	const Vector4 BLUE = Vector4(0, 0, 1, 1);

	body = new SceneNode(cube, RED);
	body->setScale(Vector3(10, 15, 5));
	body->setTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	addChild(body);

	head = new SceneNode(cube, GREEN);
	head->setScale(Vector3(5, 5, 5));
	head->setTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->addChild(head);

	leftArm = new SceneNode(cube, BLUE);
	leftArm->setScale(Vector3(3, -18, 3));
	leftArm->setTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->addChild(leftArm);

	rightArm = new SceneNode(cube, BLUE);
	rightArm->setScale(Vector3(3, -18, 3));
	rightArm->setTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->addChild(rightArm);

	leftLeg = new SceneNode(cube, BLUE);
	leftLeg->setScale(Vector3(3, -17.5, 3));
	leftLeg->setTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->addChild(leftLeg);

	rightLeg = new SceneNode(cube, BLUE);
	rightLeg->setScale(Vector3(3, -17.5, 3));
	rightLeg->setTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->addChild(rightLeg);
}

CubeBot::~CubeBot()
{
}

void CubeBot::onUpdate(float dt)
{
	transform = transform
		* Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	leftArm->setTransform(leftArm->getTransform()
		* Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));

	rightArm->setTransform(rightArm->getTransform()
		* Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

}
