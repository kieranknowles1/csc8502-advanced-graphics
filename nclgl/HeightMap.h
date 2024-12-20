#pragma once

#include "Mesh.h"

// Height map mesh that is procedurally generated
// from a height map image file
// Intended to be rendered using tessellation
class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& file, Vector3 vertexScale = Vector3(16, 1, 16), Vector2 textureScale = Vector2(1.0f / 16.0f, 1.0f / 16.0f), bool useTessellation = true);
	~HeightMap(void) {};

	Vector3 getSize() const { return size; }

protected:
	Vector3 size;
};

