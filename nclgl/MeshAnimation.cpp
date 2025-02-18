#include "MeshAnimation.h"
#include "Matrix4.h"

#include <fstream>
#include <string>

MeshAnimation::MeshAnimation() {
	jointCount = 0;
	frameCount = 0;
	frameRate = 0.0f;
}

MeshAnimation::MeshAnimation(const std::string& filename) : MeshAnimation() {
	name = filename;
	std::ifstream file(MESHDIR + filename);

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshAnim") {
		std::cout << "File is not a MeshAnim file!" << std::endl;
		return;
	}
	file >> fileVersion;
	file >> frameCount;
	file >> jointCount;
	file >> frameRate;

	allJoints.reserve(frameCount * jointCount);

	for (unsigned int f = 0; f < frameCount; ++f) {
		for (unsigned int j = 0; j < jointCount; ++j) {
			Matrix4 mat;
			for (int i = 0; i < 16; ++i) {
				file >> mat.values[i];
			}
			allJoints.emplace_back(mat);
		}
	}
}

MeshAnimation::~MeshAnimation() {

}

const Matrix4* MeshAnimation::GetJointData(unsigned int frame) const {
	if (frame >= frameCount) {
		return nullptr;
	}
	int matStart = frame * jointCount;

	Matrix4* dataStart = (Matrix4*)allJoints.data();

	return dataStart + matStart;
}