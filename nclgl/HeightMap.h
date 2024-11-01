#pragma once

#include "Mesh.h"

// Height map mesh that is procedurally generated
class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& file, int layers = 1, Vector3 vertexScale = Vector3(16, 1, 16), Vector2 textureScale = Vector2(1.0f / 16.0f, 1.0f / 16.0f));
	~HeightMap(void) {};

	Vector3 getSize() const { return size; }

protected:
	Vector3 size;
};

