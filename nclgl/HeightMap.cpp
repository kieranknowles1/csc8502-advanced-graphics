#include "HeightMap.h"

#include <cassert>

int vertex(int x, int y, int width) {
	return x + (y * width);
}

HeightMap::HeightMap(const std::string& file, int layers, Vector3 vertexScale, Vector2 textureScale)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	// TODO: Scale by number of layers

	// Load the image data as greyscale
	unsigned char* data = SOIL_load_image(file.c_str(), &width, &height, &channels, SOIL_LOAD_L);

	if (data == nullptr) {
		throw std::runtime_error("Failed to load heightmap image");
	}

	int numVertices = width * height;
	// -1 as we can't get a position from the last row/column
	int numIndices = (width - 1) * (height - 1) * 6;

	vertices.resize(numVertices);
	textureCoords.resize(numVertices);
	colours.resize(numVertices);
	indices.resize(numIndices);

	// Iterate over each pixel, treating each pixel as a vertex
	for (int x = 0; x < width; x++) {
		for (int z = 0; z < height; z++) {
			int offset = (x + (z * width));
			assert(offset < numVertices);

			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;

			colours[offset] = Vector4(1.0, 1.0, 1.0, 1.0);
		}
	}
	SOIL_free_image_data(data);

	// Create indices as a series of patches, each patch being two triangles
	int i = 0;
	for (int z = 0; z < height - 1; z++) {
		for (int x = 0; x < width - 1; x++) {
			int a = vertex(x, z, width);
			int b = vertex(x + 1, z, width);
			int c = vertex(x + 1, z + 1, width);
			int d = vertex(x, z + 1, width);

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	generateNormals();
	generateTangents();
	BufferData();
	size.x = width * vertexScale.x;
	size.y = 256 * vertexScale.y; // Height is 8-bit
	size.z = height * vertexScale.z;
}
