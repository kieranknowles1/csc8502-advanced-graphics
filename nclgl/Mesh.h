/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents.


-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	,
	COLOUR_BUFFER	,
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	,
	TANGENT_BUFFER	,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER	,

	MAX_BUFFER
};

class Mesh	{
public:
	// Generate a simple mesh representing a triangle
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();
	// Generate a simple mesh representing a cube using lines, for debugging
	static Mesh* LoadDebugCube();

	struct SubMesh {
		int start;
		int count;
	};

	Mesh(void);
	~Mesh(void);

	void Draw() const;
	void DrawSubMesh(int i);

	static Mesh* LoadFromMeshFile(const std::string& name);

	unsigned int GetTriCount() const {
		int primCount = indices.size() ? numIndices : numVertices;
		return primCount / 3;
	}

	unsigned int getVertexCount() const {
		return numVertices;
	}
	const Vector3& getVertex(int i) const {
		return vertices[i];
	}

	unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}


	int GetIndexForJoint(const std::string& name) const;
	int GetParentForJoint(const std::string& name) const;
	int GetParentForJoint(int i) const;

	const std::vector<Matrix4>& GetBindPose() const {
		return bindPose;
	}

	const std::vector<Matrix4>& GetInverseBindPose() const {
		return inverseBindPose;
	}

	int		GetSubMeshCount() const {
		return (int)meshLayers.size();
	}

	bool GetSubMesh(int i, const SubMesh* s) const;
	bool GetSubMesh(const std::string& name, const SubMesh* s) const;

	// Fill vertex normals based on triangles
	void generateNormals();

	// Fill vertex tangents based on triangles
	void generateTangents();

protected:
	// Fill a, b, and c with the vertex indices for the i-th triangle
	// Returns false if i is out of bounds
	bool getVertexIndeciesForTri(unsigned int i, unsigned int* a, unsigned int* b, unsigned int* c) const;

	// Generate the tangent for the triangle with vertices a, b, and c
	Vector4 generateTangent(int a, int b, int c);

	void	BufferData();

	GLuint	arrayObject;

	GLuint	bufferObject[MAX_BUFFER];

	GLuint	numVertices;
	GLuint	numIndices;

	GLuint	type;

	std::vector<Vector3>		vertices;
	std::vector<Vector4>		colours;
	std::vector<Vector2>		textureCoords;
	std::vector<Vector3>		normals;
	std::vector<Vector4>		tangents;

	std::vector<Vector4>		weights;
	std::vector<int>		weightIndices;

	std::vector<unsigned int>	indices;

	std::vector<Matrix4>		bindPose;
	std::vector<Matrix4>		inverseBindPose;

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
	std::vector< SubMesh>		meshLayers;
	std::vector<std::string>	layerNames;
};

