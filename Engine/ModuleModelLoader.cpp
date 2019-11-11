#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "Texture.h"

#include <limits>       // std::numeric_limits
#include <algorithm>    // std::max

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
	LoadModel(HOUSE_MODEL_PATH);
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
	UnloadCurrentModel();
	return true;
}


void ModuleModelLoader::SwapCurrentModel(const char *new_model_file_path)
{
	UnloadCurrentModel();
	LoadModel(new_model_file_path);
}

void ModuleModelLoader::UnloadCurrentModel()
{
	glDeleteTextures(scene->mNumMaterials, textures);
	delete[] textures;

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		delete meshes[i];
	}
	meshes.clear();

	delete model_bounding_box;

	//delete scene; // TODO: Why this is memory leak

}

bool ModuleModelLoader::LoadModel(const char *new_model_file_path)
{
	LOG("Assimp: Loading model %s", new_model_file_path);
	scene = aiImportFile(new_model_file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		LOG("Assimp: Error loading model %s ", new_model_file_path);
		LOG(error);
		return false;
	}

	LOG("Loading model materials");
	textures = new GLuint[scene->mNumMaterials];
	glGenTextures(scene->mNumMaterials, textures);
	std::string model_base_path = GetModelBasePath(new_model_file_path);
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		LOG("Loading material %d", i);
		LoadMaterialData(scene->mMaterials[i], textures[i], model_base_path);
	}

	LOG("Loading model meshes");
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		LOG("Loading mesh %d", i);
		LoadMeshData(scene->mMeshes[i]);
	}

	LOG("Computing model bounding box");
	ComputeBoundingBox();
	App->cameras->Center(model_bounding_box);
	return true;
}

void ModuleModelLoader::LoadMeshData(const aiMesh *mesh)
{
	std::vector<Mesh::Vertex> vertices;
	LOG("Loading mesh vertices");
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Mesh::Vertex vertex;
		vertex.position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}

	LOG("Loading mesh indices");
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	LOG("Mesh uses material %d", mesh->mMaterialIndex);
	std::vector<unsigned int> mesh_textures;
	mesh_textures.push_back(textures[mesh->mMaterialIndex]);

	meshes.push_back(new Mesh(vertices, indices, mesh_textures));
}

void ModuleModelLoader::LoadMaterialData(const aiMaterial *material, const GLuint &texture, std::string model_base_path)
{
	aiString file;
	aiTextureMapping mapping = aiTextureMapping_UV;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);

	LOG("Loading material texture in described path %s.", file.data);
	if (App->texture->loadTexture(file.data, texture))
	{
		LOG("Material loaded correctly.");
		return;
	}

	model_base_path = model_base_path + file.data;
	LOG("Loading material texture in model folder path %s.", model_base_path.c_str());
	if (App->texture->loadTexture(model_base_path.c_str(), texture))
	{
		LOG("Material loaded correctly.");
		return;
	}

	std::string textures_path = std::string("./Textures/") + file.data;
	LOG("Loading material texture in textures folder %s.", textures_path.c_str());
	if (App->texture->loadTexture(textures_path.c_str(), texture))
	{
		LOG("Material loaded correctly.");
		return;
	}
}

void ModuleModelLoader::ComputeBoundingBox()
{
	const float infinite = std::numeric_limits<float>::infinity();

	float3 min_coordinates = float3(infinite);
	float3 max_coordinates = float3(-infinite);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		for (unsigned int j = 0; j < meshes[i]->vertices.size(); ++j)
		{
			if (meshes[i]->vertices[j].position.x > max_coordinates.x)
			{
				max_coordinates.x = meshes[i]->vertices[j].position.x;
			}
			if (meshes[i]->vertices[j].position.x < min_coordinates.x)
			{
				min_coordinates.x = meshes[i]->vertices[j].position.x;
			}


			if (meshes[i]->vertices[j].position.y > max_coordinates.y)
			{
				max_coordinates.y = meshes[i]->vertices[j].position.y;
			}
			if (meshes[i]->vertices[j].position.y < min_coordinates.y)
			{
				min_coordinates.y = meshes[i]->vertices[j].position.y;
			}


			if (meshes[i]->vertices[j].position.z > max_coordinates.z)
			{
				max_coordinates.z = meshes[i]->vertices[j].position.z;
			}
			if (meshes[i]->vertices[j].position.z < min_coordinates.z)
			{
				min_coordinates.z = meshes[i]->vertices[j].position.z;
			}
			
		}
	}

	model_bounding_box = new BoundingBox(min_coordinates, max_coordinates);
}

std::string ModuleModelLoader::GetModelBasePath(const char *model_file_path) const
{
	std::string file_string = std::string(model_file_path);
	
	std::size_t found = file_string.find_last_of("/\\");
	std::string model_base_path = file_string.substr(0, found + 1);

	return model_base_path;
}