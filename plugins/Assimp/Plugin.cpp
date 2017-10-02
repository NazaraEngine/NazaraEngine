/*
Nazara Engine - Assimp Plugin

Copyright (C) 2015 Jérôme "Lynix" Leclercq (lynix680@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <CustomStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <assimp/cfileio.h>
#include <assimp/cimport.h>
#include <assimp/config.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <set>

using namespace Nz;

void ProcessJoints(aiNode* node, Skeleton* skeleton, const std::set<Nz::String>& joints)
{
	Nz::String jointName(node->mName.data, node->mName.length);
	if (joints.count(jointName))
	{
		Joint* joint = skeleton->GetJoint(jointName);
		if (joint)
		{
			if (node->mParent)
				joint->SetParent(skeleton->GetJoint(node->mParent->mName.C_Str()));

			Matrix4f transformMatrix(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
			                         node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
			                         node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
			                         node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);

			transformMatrix.InverseAffine();

			joint->SetInverseBindMatrix(transformMatrix);
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ProcessJoints(node->mChildren[i], skeleton, joints);
}

bool IsSupported(const String& extension)
{
	String dotExt = '.' + extension;
	return (aiIsExtensionSupported(dotExt.GetConstBuffer()) == AI_TRUE);
}

Ternary Check(Stream& /*stream*/, const MeshParams& parameters)
{
	bool skip;
	if (parameters.custom.GetBooleanParameter("SkipAssimpLoader", &skip) && skip)
		return Ternary_False;

	return Ternary_Unknown;
}

bool Load(Mesh* mesh, Stream& stream, const MeshParams& parameters)
{
	Nz::String streamPath = stream.GetPath();

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.IsEmpty()) ? streamPath.GetConstBuffer() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = aiProcess_CalcTangentSpace     | aiProcess_JoinIdenticalVertices
	                         | aiProcess_MakeLeftHanded       | aiProcess_Triangulate
	                         | aiProcess_RemoveComponent      | aiProcess_GenSmoothNormals
	                         | aiProcess_SplitLargeMeshes     | aiProcess_LimitBoneWeights
	                         | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials
	                         | aiProcess_FixInfacingNormals   | aiProcess_SortByPType
	                         | aiProcess_FindInvalidData      | aiProcess_GenUVCoords
	                         | aiProcess_TransformUVCoords    | aiProcess_OptimizeMeshes
	                         | aiProcess_OptimizeGraph        | aiProcess_FlipWindingOrder
	                         | aiProcess_Debone;

	if (parameters.optimizeIndexBuffers)
		postProcess |= aiProcess_ImproveCacheLocality;

	double smoothingAngle = 80.f;
	parameters.custom.GetDoubleParameter("AssimpLoader_SmoothingAngle", &smoothingAngle);

	long long triangleLimit = 1'000'000;
	parameters.custom.GetIntegerParameter("AssimpLoader_TriangleLimit", &triangleLimit);

	long long vertexLimit   = 1'000'000;
	parameters.custom.GetIntegerParameter("AssimpLoader_VertexLimit", &vertexLimit);

	aiPropertyStore* properties = aiCreatePropertyStore();
	aiSetImportPropertyFloat(properties,   AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, float(smoothingAngle));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_LBW_MAX_WEIGHTS,         4);
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SBP_REMOVE,              ~aiPrimitiveType_TRIANGLE); //< We only want triangles
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,      int(triangleLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_VERTEX_LIMIT,        int(vertexLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_RVC_FLAGS,               aiComponent_COLORS);

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	aiReleasePropertyStore(properties);

	if (!scene)
	{
		NazaraError("Assimp failed to import file: " + Nz::String(aiGetErrorString()));
		return false;
	}

	std::set<Nz::String> joints;

	bool animatedMesh = false;
	if (parameters.animated)
	{
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			aiMesh* currentMesh = scene->mMeshes[i];
			if (currentMesh->HasBones()) // Inline functions can be safely called
			{
				animatedMesh = true;
				for (unsigned int j = 0; j < currentMesh->mNumBones; ++j)
					joints.insert(currentMesh->mBones[j]->mName.C_Str());
			}
		}
	}

	if (animatedMesh)
	{
		mesh->CreateSkeletal(UInt32(joints.size()));

		Skeleton* skeleton = mesh->GetSkeleton();

		// First, assign names
		unsigned int jointIndex = 0;
		for (const Nz::String& jointName : joints)
			skeleton->GetJoint(jointIndex++)->SetName(jointName);

		ProcessJoints(scene->mRootNode, skeleton, joints);

		return false;
	}
	else
	{
		mesh->CreateStatic();

		// aiMaterial index in scene => Material index and data in Mesh
		std::unordered_map<unsigned int, std::pair<UInt32, ParameterList>> materials;

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			aiMesh* iMesh = scene->mMeshes[i];
			if (!iMesh->HasBones()) // Don't process skeletal meshs
			{
				unsigned int indexCount = iMesh->mNumFaces * 3;
				unsigned int vertexCount = iMesh->mNumVertices;

				// Index buffer
				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				IndexBufferRef indexBuffer = IndexBuffer::New(largeIndices, indexCount, parameters.storage, 0);

				IndexMapper indexMapper(indexBuffer, BufferAccess_DiscardAndWrite);
				IndexIterator index = indexMapper.begin();

				for (unsigned int j = 0; j < iMesh->mNumFaces; ++j)
				{
					aiFace& face = iMesh->mFaces[j];
					if (face.mNumIndices != 3)
						NazaraWarning("Assimp plugin: This face is not a triangle!");

					*index++ = face.mIndices[0];
					*index++ = face.mIndices[1];
					*index++ = face.mIndices[2];
				}
				indexMapper.Unmap();

				// Vertex buffer

				// Make sure the normal/tangent matrix won't rescale our vectors
				Nz::Matrix4f normalTangentMatrix = parameters.matrix;
				if (normalTangentMatrix.HasScale())
					normalTangentMatrix.ApplyScale(1.f / normalTangentMatrix.GetScale());

				VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent), vertexCount, parameters.storage, 0);
				BufferMapper<VertexBuffer> vertexMapper(vertexBuffer, BufferAccess_WriteOnly);

				MeshVertex* vertex = static_cast<MeshVertex*>(vertexMapper.GetPointer());
				for (unsigned int j = 0; j < vertexCount; ++j)
				{
					aiVector3D position = iMesh->mVertices[j];
					aiVector3D normal = iMesh->mNormals[j];
					aiVector3D tangent = (iMesh->HasTangentsAndBitangents()) ? iMesh->mTangents[j] : aiVector3D(0.f, 1.f, 0.f);
					aiVector3D uv = (iMesh->HasTextureCoords(0)) ? iMesh->mTextureCoords[0][j] : aiVector3D(0.f);

					vertex->position = parameters.matrix * Vector3f(position.x, position.y, position.z);
					vertex->normal = normalTangentMatrix.Transform({normal.x, normal.y, normal.z}, 0.f);
					vertex->tangent = normalTangentMatrix.Transform({tangent.x, tangent.y, tangent.z}, 0.f);
					vertex->uv.Set(parameters.texCoordOffset + Vector2f(uv.x, uv.y) * parameters.texCoordScale);
					vertex++;
				}

				vertexMapper.Unmap();

				// Submesh
				StaticMeshRef subMesh = StaticMesh::New(mesh);
				subMesh->Create(vertexBuffer);

				subMesh->SetIndexBuffer(indexBuffer);
				subMesh->GenerateAABB();
				subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

				auto matIt = materials.find(iMesh->mMaterialIndex);
				if (matIt == materials.end())
				{
					ParameterList matData;
					aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

					auto ConvertColor = [&] (const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
					{
						aiColor4D color;
						if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
						{
							matData.SetParameter(colorKey, Color(static_cast<UInt8>(color.r * 255), static_cast<UInt8>(color.g * 255), static_cast<UInt8>(color.b * 255), static_cast<UInt8>(color.a * 255)));
						}
					};

					auto ConvertTexture = [&] (aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
					{
						aiString path;
						aiTextureMapMode mapMode[3];
						if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
						{
							matData.SetParameter(textureKey, stream.GetDirectory() + String(path.data, path.length));

							if (wrapKey)
							{
								SamplerWrap wrap = SamplerWrap_Default;
								switch (mapMode[0])
								{
									case aiTextureMapMode_Clamp:
									case aiTextureMapMode_Decal:
										wrap = SamplerWrap_Clamp;
										break;

									case aiTextureMapMode_Mirror:
										wrap = SamplerWrap_MirroredRepeat;
										break;

									case aiTextureMapMode_Wrap:
										wrap = SamplerWrap_Repeat;
										break;

									default:
										NazaraWarning("Assimp texture map mode 0x" + String::Number(mapMode[0], 16) + " not handled");
										break;
								}

								matData.SetParameter(wrapKey, static_cast<long long>(wrap));
							}
						}
					};

					ConvertColor(AI_MATKEY_COLOR_AMBIENT, MaterialData::AmbientColor);
					ConvertColor(AI_MATKEY_COLOR_DIFFUSE, MaterialData::DiffuseColor);
					ConvertColor(AI_MATKEY_COLOR_SPECULAR, MaterialData::SpecularColor);

					ConvertTexture(aiTextureType_DIFFUSE, MaterialData::DiffuseTexturePath, MaterialData::DiffuseWrap);
					ConvertTexture(aiTextureType_EMISSIVE, MaterialData::EmissiveTexturePath);
					ConvertTexture(aiTextureType_HEIGHT, MaterialData::HeightTexturePath);
					ConvertTexture(aiTextureType_NORMALS, MaterialData::NormalTexturePath);
					ConvertTexture(aiTextureType_OPACITY, MaterialData::AlphaTexturePath);
					ConvertTexture(aiTextureType_SPECULAR, MaterialData::SpecularTexturePath, MaterialData::SpecularWrap);

					aiString name;
					if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
						matData.SetParameter(MaterialData::Name, String(name.data, name.length));

					int iValue;
					if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
						matData.SetParameter(MaterialData::FaceCulling, !iValue);

					matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(UInt32(materials.size()), std::move(matData)))).first;
				}

				subMesh->SetMaterialIndex(matIt->first);

				mesh->AddSubMesh(subMesh);
			}

			mesh->SetMaterialCount(std::max<UInt32>(UInt32(materials.size()), 1));
			for (const auto& pair : materials)
				mesh->SetMaterialData(pair.second.first, pair.second.second);
		}

		if (parameters.center)
			mesh->Recenter();
	}

	aiReleaseImport(scene);

	return true;
}

extern "C"
{
	NAZARA_EXPORT int PluginLoad()
	{
		Nz::MeshLoader::RegisterLoader(IsSupported, Check, Load);
		return 1;
	}

	NAZARA_EXPORT void PluginUnload()
	{
		Nz::MeshLoader::UnregisterLoader(IsSupported, Check, Load);
	}
}
