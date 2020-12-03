#include "MeshImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/Skeleton.h"
#include "Helper/Utils.h"
#include <map>

FileData MeshImporter::ExtractMeshFromAssimp(const aiMesh* mesh, const aiMatrix4x4& mesh_current_transformation, float unit_scale_factor, uint32_t mesh_skeleton_uuid, bool animated_model) const
{
	FileData mesh_data{NULL, 0};

	// Transformation
	aiMatrix4x4 scaling_matrix = aiMatrix4x4() * unit_scale_factor;
	scaling_matrix.d4 = 1;

	aiMatrix4x4 node_transformation = mesh_current_transformation;

	node_transformation = scaling_matrix * node_transformation;

	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//We only accept triangle formed meshes
	if (indices.size() % 3 != 0)
	{
		RESOURCES_LOG_ERROR("Mesh %s have incorrect indices", mesh->mName.C_Str());
		return mesh_data;
	}

	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> vertex_skinning__info;
	if (mesh->HasBones() && mesh_skeleton_uuid != 0)
	{
		vertex_skinning__info = GetSkinning(mesh, mesh_skeleton_uuid);
	}

	float3x3 node_rotation = float3x3::zero;
	aiMatrix4x4 vertex_transformation;
	if (animated_model)
	{
		vertex_transformation = node_transformation;
		node_rotation = Utils::GetTransform(mesh_current_transformation, 1.0f).RotatePart();
	}
	else
	{
		vertex_transformation = scaling_matrix;
		node_rotation = Quat::identity.ToFloat3x3();
	}

	std::vector<Mesh::Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Mesh::Vertex new_vertex;
		aiVector3D transformed_position = vertex_transformation * mesh->mVertices[i];
		new_vertex.position = float3(transformed_position.x, transformed_position.y, transformed_position.z);


		for (size_t j = 0; j < UVChannel::TOTALUVS; j++)
		{
			float2 text_coordinate(float2::zero);
			if (mesh->mTextureCoords[j])
			{
				text_coordinate = float2(mesh->mTextureCoords[j][i].x, mesh->mTextureCoords[j][i].y);
			}
			new_vertex.tex_coords[j] = text_coordinate;
		}
		if (mesh->mNormals)
		{
			new_vertex.normals = (node_rotation * float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)).Normalized();
		}
		if (mesh->mTangents)
		{
			new_vertex.tangent = (node_rotation * float3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)).Normalized();
		}
		if (mesh->mBitangents)
		{
			new_vertex.bitangent = (node_rotation * float3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)).Normalized();
		}
		if (vertex_skinning__info.size() > 0)
		{
			assert(vertex_skinning__info[i].first.size() <= MAX_JOINTS);
			for (size_t j = 0; j < vertex_skinning__info[i].first.size(); ++j)
			{
				new_vertex.joints[j] = vertex_skinning__info[i].first[j];
			}
			assert(vertex_skinning__info[i].second.size() <= MAX_JOINTS);

			float weights_sum = 0;
			for (size_t j = 0; j < vertex_skinning__info[i].second.size(); ++j)
			{
				weights_sum += vertex_skinning__info[i].second[j];
			}
			
			float normalize_factor = 1.0f / weights_sum;
			weights_sum = 0;
			for (size_t j = 0; j < vertex_skinning__info[i].second.size(); ++j)
			{
				new_vertex.weights[j] = vertex_skinning__info[i].second[j] * normalize_factor;
				weights_sum += new_vertex.weights[j];
			}
			float weights_sum_round = std::round(weights_sum);
			assert(weights_sum_round <= 1.0f && weights_sum_round >= 0.0f);
			new_vertex.num_joints = vertex_skinning__info[i].second.size();
		}
		vertices.push_back(new_vertex);
	}

	return CreateBinary(std::move(vertices), std::move(indices));
}

std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> MeshImporter::GetSkinning(const aiMesh* mesh, uint32_t mesh_skeleton_uuid) const
{
	std::shared_ptr<Skeleton> skeleton = App->resources->Load<Skeleton>(mesh_skeleton_uuid);
	std::vector<std::pair<std::vector<uint32_t>, std::vector<float>>> vertex_weights_joint(mesh->mNumVertices);
	for (size_t j = 0; j < mesh->mNumBones; ++j)
	{
		aiBone * mesh_bone = mesh->mBones[j];
		std::string mesh_bone_name(mesh_bone->mName.C_Str()); //TODO: Change to use hash instead of name everywhere (Mesh, animation, skeleton)
		const auto it = std::find_if(skeleton->skeleton.begin(), skeleton->skeleton.end(), [&mesh_bone_name](const Skeleton::Joint & joint)
		{
			return joint.name == mesh_bone_name;
		});
		assert(it != skeleton->skeleton.end()); //If you are here, probably you are loading a fbx where some bone is holding a mesh
		for (size_t k = 0; k < mesh_bone->mNumWeights; ++k)
		{
			aiVertexWeight vertex_weight = mesh_bone->mWeights[k];
			assert(vertex_weight.mVertexId <= vertex_weights_joint.size());
			vertex_weights_joint[vertex_weight.mVertexId].second.push_back(vertex_weight.mWeight);
			vertex_weights_joint[vertex_weight.mVertexId].first.push_back(it - skeleton->skeleton.begin());
		}
	}
	return vertex_weights_joint;
}

FileData MeshImporter::CreateBinary(std::vector<Mesh::Vertex> && vertices, std::vector<uint32_t> && indices) const
{

	uint32_t num_indices = indices.size();
	uint32_t num_vertices = vertices.size();
	uint32_t ranges[2] = { num_indices, num_vertices };

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

	FileData mesh_data {data, size};
	return mesh_data;
}