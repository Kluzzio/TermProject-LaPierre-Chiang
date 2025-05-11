#include "mesh.h"

Mesh::Mesh()
{
	// Vertex Set Up
	// No mesh

	// Model Set Up
	angle = 0.0f;
	pivotLocation = glm::vec3(0.f, 0.f, 0.f);
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("Some buffers not initialized.\n");
	}

}

Mesh::Mesh(glm::vec3 pivot, const char* fname)
{
	// Vertex Set Up
	if (!loadModelFromFile(fname)) {
		printf("model could not be loaded from file"); 
	}

	// Model Set Up
	angle = 0.0f;
	pivotLocation = pivot;
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("some buffers not initialized.\n");
	}

	hasTex = false;
}

Mesh::Mesh(glm::vec3 pivot, const char* fname, const char* tname)
{
	// Vertex Set Up
	if (!loadModelFromFile(fname)) {
		printf("model could not be loaded from file");
	}


	// Model Set Up
	angle = 0.0f;
	pivotLocation = pivot;
	model = glm::translate(glm::mat4(1.0f), pivotLocation);

	// Buffer Set Up
	if (!InitBuffers()) {
		printf("some buffers not initialized.\n");
	}

	// load texture from file
	m_texture = new Texture(tname);
	if (m_texture != NULL)
		hasTex = true;
	else
		hasTex = false;
}


Mesh::~Mesh()
{
	Vertices.clear();
	Indices.clear();
}

void Mesh::Update(glm::mat4 inmodel)
{
	model = inmodel;

}

glm::mat4 Mesh::GetModel()
{
	return model;
}

void Mesh::Render(GLint posAttribLoc, GLint colAttribLoc)
{

	glBindVertexArray(vao);

	// Enable vertex attibute arrays for each vertex attrib
	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(colAttribLoc);

	// Bind your VBO
	glBindBuffer(GL_ARRAY_BUFFER, VB);

	// Set vertex attribute pointers to the load correct data
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Bind your Element Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	// Render
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	// Disable vertex arrays
	glDisableVertexAttribArray(posAttribLoc);
	glDisableVertexAttribArray(colAttribLoc);
}

void Mesh::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc)
{
	glBindVertexArray(vao);
	// Enable vertex attibute arrays for each vertex attrib
	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(colAttribLoc);
	glEnableVertexAttribArray(tcAttribLoc);

	// Bind your VBO
	glBindBuffer(GL_ARRAY_BUFFER, VB);

	// Set vertex attribute pointers to the load correct data
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

	// If has texture, set up texture unit(s) Update here to activate and assign texture unit
	if (hasTex) {
		glUniform1i(hasTextureLoc, true);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());
	}
	else
		glUniform1i(hasTextureLoc, false);


	// Bind your Element Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

	// Render
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	// Disable vertex arrays
	glDisableVertexAttribArray(posAttribLoc);
	glDisableVertexAttribArray(colAttribLoc);
	glDisableVertexAttribArray(tcAttribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


bool Mesh::InitBuffers() {

	// For OpenGL 3
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}

bool Mesh::loadModelFromFile(const char* path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (!scene) {
		printf("couldn't open the .obj file. \n");
		return false;
	}

	const int ivertTotalSize = 2 * sizeof(aiVector3D);

	int iTotalVerts = 0;

	for (int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		bool hasTC = (mesh->mTextureCoords[0] != NULL);
		int iMeshFaces = mesh->mNumFaces;
		for (int j = 0; j < iMeshFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < 3; k++) {
				int index = face.mIndices[k];
				glm::vec3 vertex = { mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z };
				glm::vec3 norm = { mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z };
				glm::vec2 tex;
				hasTC ? tex = { mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y } : tex = { 0., 0. };
				Vertices.push_back(Vertex(vertex, norm, tex));
			}

		}
		iTotalVerts += mesh->mNumVertices;
	}
	for (int i = 0; i < Vertices.size(); i++) {
		Indices.push_back(i);
	}
	return true;
}
