#include "Mesh.h"
#include "Matrix2.h"

using std::string;

Mesh* Mesh::GenerateTriangle() {
	Mesh* m = new Mesh();
	m->name = "TrianglePrimitive";

	m->vertices.emplace_back(0.0f, 0.5f, 0.0f);
	m->vertices.emplace_back(0.5f, -0.5f, 0.0f);
	m->vertices.emplace_back(-0.5f, -0.5f, 0.0f);

	m->colours.emplace_back(1.0f, 0.0f, 0.0f, 1.0f); // Red
	m->colours.emplace_back(0.0f, 1.0f, 0.0f, 1.0f); // Green
	m->colours.emplace_back(0.0f, 0.0f, 1.0f, 1.0f); // Blue

	m->textureCoords.emplace_back(0.5f, 0.0f);
	m->textureCoords.emplace_back(1.0f, 1.0f);
	m->textureCoords.emplace_back(0.0f, 1.0f);

	// Copy data to GPU
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuad() {
	Mesh* m = new Mesh();
	m->name = "QuadPrimitive";
	m->type = GL_TRIANGLE_STRIP; // Draw a triangle after each point, using the last two points and the new one

	m->vertices.emplace_back(-1.0f, 1.0f, 0.0f); // Top left
	m->textureCoords.emplace_back(0.0f, 1.0f);

	m->vertices.emplace_back(-1.0f, -1.0f, 0.0f); // Bottom left
	m->textureCoords.emplace_back(0.0f, 0.0f);

	m->vertices.emplace_back(1.0f, 1.0f, 0.0f); // Top right
	m->textureCoords.emplace_back(1.0f, 1.0f);

	m->vertices.emplace_back(1.0f, -1.0f, 0.0f); // Bottom right
	m->textureCoords.emplace_back(1.0f, 0.0f);

	for (int i = 0; i < 4; i++) {
		m->colours.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
		m->normals.emplace_back(0.0f, 0.0f, -1.0f); // Point down
		m->tangents.emplace_back(1.0f, 0.0f, 0.0f, 1.0f); // Point right
	}

	m->BufferData();
	return m;
}

Mesh::Mesh(void)	{
	glGenVertexArrays(1, &arrayObject);

	for(int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}

	type		 = GL_TRIANGLES;
}

Mesh::~Mesh(void)	{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs
}

void Mesh::Draw() const {
	glBindVertexArray(arrayObject);
	if (type == GL_PATCHES) {
		glPatchParameteri(GL_PATCH_VERTICES, 4);
	}
	if(bufferObject[INDEX_BUFFER]) {
		glDrawElements(type, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else{
		glDrawArrays(type, 0, vertices.size());
	}
	glBindVertexArray(0);
}

void Mesh::DrawSubMesh(int i) {
	if (i < 0 || i >= (int)meshLayers.size()) {
		return;
	}
	SubMesh m = meshLayers[i];

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) {
		const GLvoid* offset = (const GLvoid * )(m.start * sizeof(unsigned int));
		glDrawElements(type, m.count, GL_UNSIGNED_INT, offset);
	}
	else {
		glDrawArrays(type, m.start, m.count);	//Draw the triangle!
	}
	glBindVertexArray(0);
}

void UploadAttribute(GLuint* id, int numElements, int dataSize, int attribSize, int attribID, void* pointer, const string&debugName) {
	glGenBuffers(1, id);
	glBindBuffer(GL_ARRAY_BUFFER, *id);
	glBufferData(GL_ARRAY_BUFFER, numElements * dataSize, pointer, GL_STATIC_DRAW);

	glVertexAttribPointer(attribID, attribSize, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribID);

	glObjectLabel(GL_BUFFER, *id, -1, debugName.c_str());
}

bool Mesh::getVertexIndeciesForTri(unsigned int i, unsigned int* a, unsigned int* b, unsigned int* c) const
{
	auto triCount = GetTriCount();
	if (i >= triCount) return false;

	unsigned int index = i * 3;
	if (indices.size()) {
		*a = indices[index];
		*b = indices[index + 1];
		*c = indices[index + 2];
	}
	else {
		*a = index;
		*b = index + 1;
		*c = index + 2;
	}
	return true;
}

void	Mesh::BufferData()	{
	glBindVertexArray(arrayObject);

	////Buffer vertex data
	UploadAttribute(&bufferObject[VERTEX_BUFFER], vertices.size(), sizeof(Vector3), 3, VERTEX_BUFFER, vertices.data(), "Positions");

	if(textureCoords.size()) {	//Buffer texture data
		UploadAttribute(&bufferObject[TEXTURE_BUFFER], textureCoords.size(), sizeof(Vector2), 2, TEXTURE_BUFFER, textureCoords.data(), "TexCoords");
	}

	if (colours.size()) {
		UploadAttribute(&bufferObject[COLOUR_BUFFER], colours.size(), sizeof(Vector4), 4, COLOUR_BUFFER, colours.data(), "Colours");
	}

	if (normals.size()) {	//Buffer normal data
		UploadAttribute(&bufferObject[NORMAL_BUFFER], normals.size(), sizeof(Vector3), 3, NORMAL_BUFFER, normals.data(), "Normals");
	}

	if (tangents.size()) {	//Buffer tangent data
		UploadAttribute(&bufferObject[TANGENT_BUFFER], tangents.size(), sizeof(Vector4), 4, TANGENT_BUFFER, tangents.data(), "Tangents");
	}

	if (weights.size()) {		//Buffer weights data
		UploadAttribute(&bufferObject[WEIGHTVALUE_BUFFER], weights.size(), sizeof(Vector4), 4, WEIGHTVALUE_BUFFER, weights.data(), "Weights");
	}

	//Buffer weight indices data...uses a different function since its integers...
	if (weightIndices.size()) {
		glGenBuffers(1, &bufferObject[WEIGHTINDEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[WEIGHTINDEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, weightIndices.size() * sizeof(int) * 4, weightIndices.data(), GL_STATIC_DRAW);
		glVertexAttribIPointer(WEIGHTINDEX_BUFFER, 4, GL_INT, 0, 0); //note the new function...
		glEnableVertexAttribArray(WEIGHTINDEX_BUFFER);

		glObjectLabel(GL_BUFFER, bufferObject[WEIGHTINDEX_BUFFER], -1, "Weight Indices");
	}

	//buffer index data
	if(indices.size()) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		glObjectLabel(GL_BUFFER, bufferObject[INDEX_BUFFER], -1, "Indices");
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


/*
*
* Extra file loading stuff!
*
* */

enum class GeometryChunkTypes {
	VPositions		= 1,
	VNormals		= 2,
	VTangents		= 4,
	VColors			= 8,
	VTex0			= 16,
	VTex1			= 32,
	VWeightValues	= 64,
	VWeightIndices	= 128,
	Indices			= 256,
	JointNames		= 512,
	JointParents	= 1024,
	BindPose		= 2048,
	BindPoseInv		= 4096,
	Material		= 65536,
	SubMeshes		= 1 << 14,
	SubMeshNames	= 1 << 15
};

void ReadTextFloats(std::ifstream& file, vector<Vector2>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector2 temp;
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector3>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector3 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector4>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector4 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadTextVertexIndices(std::ifstream& file, vector<int>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		int indices[4];
		file >> indices[0];
		file >> indices[1];
		file >> indices[2];
		file >> indices[3];
		element.emplace_back(indices[0]);
		element.emplace_back(indices[1]);
		element.emplace_back(indices[2]);
		element.emplace_back(indices[3]);
	}
}

void ReadIndices(std::ifstream& file, vector<unsigned int>& elements, int numIndices) {
	for (int i = 0; i < numIndices; ++i) {
		unsigned int temp;
		file >> temp;
		elements.emplace_back(temp);
	}
}

void ReadJointParents(std::ifstream& file, vector<int>& dest) {
	int jointCount = 0;
	file >> jointCount;

	for (int i = 0; i < jointCount; ++i) {
		int id = -1;
		file >> id;
		dest.emplace_back(id);
	}
}

void ReadJointNames(std::ifstream& file, vector<string>& dest) {
	int jointCount = 0;
	file >> jointCount;
	for (int i = 0; i < jointCount; ++i) {
		std::string jointName;
		file >> jointName;
		dest.emplace_back(jointName);
	}
}

void ReadRigPose(std::ifstream& file, std::vector<Matrix4>& into) {
	int matCount = 0;
	file >> matCount;

	for (int i = 0; i < matCount; ++i) {
		Matrix4 mat;
		for (int i = 0; i < 16; ++i) {
			file >> mat.values[i];
		}
		into.emplace_back(mat);
	}
}

void ReadSubMeshes(std::ifstream& file, int count, vector<Mesh::SubMesh> & subMeshes) {
	for (int i = 0; i < count; ++i) {
		Mesh::SubMesh m;
		file >> m.start;
		file >> m.count;
		subMeshes.emplace_back(m);
	}
}

void ReadSubMeshNames(std::ifstream& file, int count, vector<string>& names) {
	std::string scrap;
	std::getline(file, scrap);

	for (int i = 0; i < count; ++i) {
		std::string meshName;
		std::getline(file, meshName);
		names.emplace_back(meshName);
	}
}

Mesh* Mesh::LoadFromMeshFile(const string& name) {
	return new Mesh(name);
}

Mesh::Mesh(const string& name)
	: Mesh()
{
	this->name = name;
	auto fullPath = MESHDIR + name;
	std::ifstream file(fullPath);
	if (!file.good()) {
		throw std::runtime_error("Failed to open mesh file: " + fullPath);
	}

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshGeometry") {
		throw std::runtime_error("File is not a MeshGeometry file!");
	}

	file >> fileVersion;

	if (fileVersion != 1) {
		throw std::runtime_error("MeshGeometry file has incompatible version!");
	}

	int numMeshes	= 0; //read
	int numVertices = 0; //read
	int numIndices	= 0; //read
	int numChunks	= 0; //read

	file >> numMeshes;
	file >> numVertices;
	file >> numIndices;
	file >> numChunks;

	for (int i = 0; i < numChunks; ++i) {
		int chunkType = (int)GeometryChunkTypes::VPositions;

		file >> chunkType;

		switch ((GeometryChunkTypes)chunkType) {
		case GeometryChunkTypes::VPositions:ReadTextFloats(file, vertices, numVertices);  break;
		case GeometryChunkTypes::VColors:	ReadTextFloats(file, colours, numVertices);  break;
		case GeometryChunkTypes::VNormals:	ReadTextFloats(file, normals, numVertices);  break;
		case GeometryChunkTypes::VTangents:	ReadTextFloats(file, tangents, numVertices);  break;
		case GeometryChunkTypes::VTex0:		ReadTextFloats(file, textureCoords, numVertices);  break;
		case GeometryChunkTypes::Indices:	ReadIndices(file, indices, numIndices); break;

		case GeometryChunkTypes::VWeightValues:		ReadTextFloats(file, weights, numVertices);  break;
		case GeometryChunkTypes::VWeightIndices:	ReadTextVertexIndices(file, weightIndices, numVertices);  break;
		case GeometryChunkTypes::JointNames:		ReadJointNames(file, jointNames);  break;
		case GeometryChunkTypes::JointParents:		ReadJointParents(file, jointParents);  break;
		case GeometryChunkTypes::BindPose:			ReadRigPose(file, bindPose);  break;
		case GeometryChunkTypes::BindPoseInv:		ReadRigPose(file, inverseBindPose);  break;
		case GeometryChunkTypes::SubMeshes: 		ReadSubMeshes(file, numMeshes, meshLayers); break;
		case GeometryChunkTypes::SubMeshNames: 		ReadSubMeshNames(file, numMeshes, layerNames); break;
		}
	}

	if (colours.empty()) {
		// If no colours are specified, default to white
		// otherwise the shader will see all black
		for (int i = 0; i < numVertices; ++i) {
			colours.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	BufferData();
}

int Mesh::GetIndexForJoint(const std::string& name) const {
	for (unsigned int i = 0; i < jointNames.size(); ++i) {
		if (jointNames[i] == name) {
			return i;
		}
	}
	return -1;
}

int Mesh::GetParentForJoint(const std::string& name) const {
	int i = GetIndexForJoint(name);
	if (i == -1) {
		return -1;
	}
	return jointParents[i];
}

int Mesh::GetParentForJoint(int i) const {
	if (i == -1 || i >= (int)jointParents.size()) {
		return -1;
	}
	return jointParents[i];
}

bool Mesh::GetSubMesh(int i, const SubMesh* s) const {
	if (i < 0 || i >= (int)meshLayers.size()) {
		return false;
	}
	s = &meshLayers[i];
	return true;
}

bool Mesh::GetSubMesh(const string& name, const SubMesh* s) const {
	for (unsigned int i = 0; i < layerNames.size(); ++i) {
		if (layerNames[i] == name) {
			return GetSubMesh(i, s);
		}
	}
	return false;
}

void Mesh::generateNormals()
{
	normals.clear();
	normals.resize(vertices.size(), Vector3(0.0f, 0.0f, 0.0f));

	int triCount = GetTriCount();
	for (int i = 0; i < triCount; i++) {
		unsigned int a;
		unsigned int b;
		unsigned int c;
		getVertexIndeciesForTri(i, &a, &b, &c);

		// Calculate the normal of the triangle
		Vector3 normal = Vector3::Cross(
			vertices[b] - vertices[a],
			vertices[c] - vertices[a]
		);

		// Add the normal to all three vertices
		normals[a] += normal;
		normals[b] += normal;
		normals[c] += normal;
	}

	// Normalise all the normals
	for (int i = 0; i < normals.size(); i++) {
		normals[i].Normalise();
	}
}

void Mesh::generateTangents()
{
	if (textureCoords.empty()) {
		throw std::runtime_error("Cannot generate tangents without texture coordinates!");
	}
	tangents.clear();
	tangents.resize(vertices.size(), Vector4(0.0f, 0.0f, 0.0f, 0.0f));

	int triCount = GetTriCount();
	for (int i = 0; i < triCount; i++) {
		unsigned int a;
		unsigned int b;
		unsigned int c;
		getVertexIndeciesForTri(i, &a, &b, &c);

		Vector4 tangent = generateTangent(a, b, c);
		tangents[a] += tangent;
		tangents[b] += tangent;
		tangents[c] += tangent;
	}

	for (GLuint i = 0; i < tangents.size(); i++) {
		// Don't normalise the handedness component
		float handedness = tangents[i].w > 0.0f ? 1.0f : -1.0f;
		tangents[i].w = 0.0f;

		tangents[i].Normalise();
		tangents[i].w = handedness;
	}
}

Vector4 Mesh::generateTangent(int a, int b, int c)
{
	// ba - The vector from a to b
	Vector3 ba = vertices[b] - vertices[a];
	Vector3 ca = vertices[c] - vertices[a];

	// tba - The vector from a to b in texture space
	Vector2 tba = textureCoords[b] - textureCoords[a];
	Vector2 tca = textureCoords[c] - textureCoords[a];

	Matrix2 texMatrix = Matrix2(tba, tca).inverse();

	Vector3 tangent = ba * texMatrix.values[0] + ca * texMatrix.values[1];
	Vector3 binormal = ba * texMatrix.values[2] + ca * texMatrix.values[3];

	Vector3 normal = Vector3::Cross(ba, ca);
	Vector3 biCross = Vector3::Cross(normal, tangent);

	float handedness = Vector3::Dot(biCross, binormal) < 0.0f ? -1.0f : 1.0f;

	return Vector4(
		tangent.x,
		tangent.y,
		tangent.z,
		handedness
	);
}
