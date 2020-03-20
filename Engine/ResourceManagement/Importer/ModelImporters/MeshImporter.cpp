#include "MeshImporter.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <assimp/mesh.h>
#include <map>

ImportResult MeshImporter::Import(const File & file, bool force) const
{
	std::string exported_file = SaveMetaFile(file.file_path, ResourceType::MESH);
	App->filesystem->Copy(file.file_path.c_str(), exported_file.c_str());
	return ImportResult{ true, exported_file };
}
ImportResult MeshImporter::ImportMesh(const aiMesh* mesh, const aiMatrix4x4& mesh_current_transformation, const std::string& imported_file, float unit_scale_factor,const Skeleton & skeleton) const
{
	aiVector3t<float> pScaling, pPosition;
	aiQuaterniont<float> pRotation;
	aiMatrix4x4 node_transformation = mesh_current_transformation;
	node_transformation.Decompose(pScaling, pRotation, pPosition);

	pPosition = pPosition * unit_scale_factor;
	pScaling = pScaling * unit_scale_factor;

	node_transformation = aiMatrix4x4(pScaling, pRotation, pPosition);

	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//We only accept triangle formed meshes
	if (indices.size() % 3 != 0)
	{
		APP_LOG_ERROR("Mesh %s have incorrect indices", mesh->mName.C_Str());
		return ImportResult();
	}
	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> vertex_skinning__info;
	if (mesh->HasBones())
	{
		vertex_skinning__info = GetSkinning(mesh, skeleton);
	}
	std::vector<Mesh::Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Mesh::Vertex new_vertex;
		aiVector3D transformed_position = node_transformation * mesh->mVertices[i];
		new_vertex.position = float3(transformed_position.x, transformed_position.y, transformed_position.z);
		if (mesh->mTextureCoords[0]) 
		{
			new_vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		if (mesh->mNormals)
		{
			new_vertex.normals = float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		if (mesh->mTangents)
		{
			new_vertex.tangent = float3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}
		assert(vertex_skinning__info[i].first.size() <= 4);
		for (size_t j = 0; j < vertex_skinning__info[i].first.size(); ++j)
		{
			new_vertex.joints[j] = vertex_skinning__info[i].first[j];
		}

		assert(vertex_skinning__info[i].second.size() <= 4);
		for (size_t j = 0; j < vertex_skinning__info[i].second.size(); ++j)
		{
			new_vertex.weights[j] = vertex_skinning__info[i].second[j];
		}
		vertices.push_back(new_vertex);
	}
	std::string exported_file = SaveMetaFile(imported_file, ResourceType::MESH);
	SaveBinary(std::move(vertices), std::move(indices), exported_file, imported_file);
	return ImportResult{true,exported_file};
}

std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> MeshImporter::GetSkinning(const aiMesh* mesh, const Skeleton & skeleton) const
{
	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> vertex_weights_joint(mesh->mNumVertices);
	for (size_t j = 0; j < mesh->mNumBones; ++j)
	{
		aiBone * mesh_bone = mesh->mBones[j];
		std::string mesh_bone_name(mesh_bone->mName.C_Str()); //TODO: Change to use hash instead of name everywhere (Mesh, animation, skeleton) just like this for better debugging
		auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&mesh_bone_name](const Skeleton::Joint & joint)
		{
			return joint.name == mesh_bone_name;
		});
		assert(it != skeleton.skeleton.end());
		for (size_t k = 0; k < mesh_bone->mNumWeights; ++k)
		{
			aiVertexWeight vertex_weight = mesh_bone->mWeights[k];
			assert(vertex_weight.mVertexId <= vertex_weights_joint.size());
			vertex_weights_joint[vertex_weight.mVertexId].second.push_back(vertex_weight.mWeight);
			vertex_weights_joint[vertex_weight.mVertexId].first.push_back(it - skeleton.skeleton.begin());
		}
	}
	return vertex_weights_joint;
}

void MeshImporter::SaveBinary(std::vector<Mesh::Vertex> && vertices, std::vector<uint32_t> && indices, const std::string& exported_file, const std::string& imported_file) const
{

	uint32_t num_indices = indices.size();
	uint32_t num_vertices = vertices.size();
	uint32_t ranges[2] = { num_indices, num_vertices };

	uint32_t total_vertex_size = sizeof(float3) * 3 + sizeof(float2) + sizeof(uint32_t) * 4 + sizeof(float) * 4;
	assert(sizeof(Mesh::Vertex) == total_vertex_size);
	uint32_t size = sizeof(ranges) + sizeof(uint32_t) * num_indices + sizeof(Mesh::Vertex) * num_vertices + sizeof(uint32_t);

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(uint32_t) * num_indices;
	memcpy(cursor, &indices.front(), bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(Mesh::Vertex) * num_vertices;
	memcpy(cursor, &vertices.front(), bytes);

	App->filesystem->Save(exported_file.c_str(), data, size);
	App->filesystem->Save(imported_file.c_str(), data, size);
	free(data);
}