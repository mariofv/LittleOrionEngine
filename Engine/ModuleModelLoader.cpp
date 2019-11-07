#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTexture.h"
#include "Texture.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

ModuleModelLoader::ModuleModelLoader()
{

}

// Destructor
ModuleModelLoader::~ModuleModelLoader()
{
}

// Called before render is available
bool ModuleModelLoader::Init()
{
	LOG("Loading house baker model");
	scene = aiImportFile(HOUSE_MODEL_PATH, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL) 
	{
		const char *error = aiGetErrorString();
		LOG("Error loading model %s", HOUSE_MODEL_PATH);
		LOG(error);
		return false;
	}

	textures = new GLuint[scene->mNumMaterials];
	glGenTextures(scene->mNumMaterials, textures);
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		LoadMaterialData(scene->mMaterials[i], textures[i]);
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		LoadMeshData(scene->mMeshes[i]);
	}

	return true;
}

update_status ModuleModelLoader::PreUpdate()
{
	

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleModelLoader::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleModelLoader::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleModelLoader::CleanUp()
{
	glDeleteTextures(scene->mNumMaterials, textures);
	delete[] textures;

	for (unsigned int i = 0; i < meshes.size(); ++i) 
	{
		delete meshes[i];
	}
	meshes.clear();

	//delete scene; // TODO: Why this is memory leak

	return true;
}

void ModuleModelLoader::LoadMeshData(const aiMesh *mesh)
{
	std::vector<Mesh::Vertex> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Mesh::Vertex vertex;
		vertex.position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	std::vector<unsigned int> mesh_textures;
	mesh_textures.push_back(textures[mesh->mMaterialIndex]);

	meshes.push_back(new Mesh(vertices, indices, mesh_textures));
}

/*
void ModuleModelLoader::LoadMeshData(const aiMesh *mesh, const GLuint &vao, const GLuint &vbo, const GLuint &ibo)
{
	glBindVertexArray(vao);

	// LOAD POSITIONS AND TEXTURE COORDINATES
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * mesh->mNumVertices, NULL, GL_STATIC_DRAW);

	// Add vertex positions
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices);

	// Add texture coordinates
	float* mapped_buffer = (float *)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, sizeof(float) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		memcpy(&mapped_buffer[i * 2], &mesh->mTextureCoords[0][i], sizeof(float)*2);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// LOAD INDICES
	int num_three_vertex_faces = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		if (mesh->mFaces[i].mNumIndices == 3) {
			++num_three_vertex_faces;
		}
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * num_three_vertex_faces, NULL, GL_STATIC_DRAW);
	unsigned int* mapped_buffer_i = (unsigned int *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * 3 * num_three_vertex_faces, GL_MAP_WRITE_BIT);
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		memcpy(&mapped_buffer_i[i * 3], &mesh->mFaces[i].mIndices, sizeof(unsigned int) * 3);
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices)); // uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
*/
void ModuleModelLoader::LoadMaterialData(const aiMaterial *material, const GLuint &texture)
{
	aiString file;
	aiTextureMapping mapping = aiTextureMapping_UV;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);

	char file_path[1024] = "./baker_house/"; //TODO: Look for model path in order to get its textures
	strcat(file_path, file.data);

	const Texture *material_texture = App->texture->loadTexture(file_path);
	//delete[] file_path; TODO: Ask what to do with this pointer

	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, material_texture->width, material_texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, material_texture->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete material_texture;
}
