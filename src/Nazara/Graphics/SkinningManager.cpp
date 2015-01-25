// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	enum ObjectType
	{
		ObjectType_SkeletalMesh,
		ObjectType_Skeleton,
	};

    struct BufferData
    {
		NzVertexBufferRef buffer;
		bool updated;
    };

    struct SkinningData
    {
    	const NzSkeletalMesh* mesh;
    	const NzSkeleton* skeleton;
    	NzVertexBuffer* buffer;
    };

	using MeshMap = std::unordered_map<const NzSkeletalMesh*, std::pair<NzSkeletalMeshConstListener, BufferData>>;
	using SkeletonMap = std::unordered_map<const NzSkeleton*, std::pair<NzSkeletonConstListener, MeshMap>>;
	SkeletonMap s_cache;
	std::vector<SkinningData> s_skinningQueue;

	class ObjectListener : public NzObjectListener
	{
		public:
			bool OnObjectDestroy(const NzRefCounted* object, int index) override
			{
				switch (index)
				{
					case ObjectType_SkeletalMesh:
					{
						for (auto& pair : s_cache)
						{
							MeshMap& meshMap = pair.second.second;
							meshMap.erase(static_cast<const NzSkeletalMesh*>(object));
						}
						break;
					}

					case ObjectType_Skeleton:
						s_cache.erase(static_cast<const NzSkeleton*>(object));
						break;
				}

				return false;
			}

			bool OnObjectModified(const NzRefCounted* object, int index, unsigned int code) override
			{
				NazaraUnused(code);

				switch (index)
				{
					case ObjectType_SkeletalMesh:
					{
						for (auto& pair : s_cache)
						{
							MeshMap& meshMap = pair.second.second;
							for (auto& pair2 : meshMap)
								pair2.second.second.updated = false;
						}
						break;
					}

					case ObjectType_Skeleton:
					{
						const NzSkeleton* skeleton = static_cast<const NzSkeleton*>(object);
						for (auto& pair : s_cache.at(skeleton).second)
							pair.second.second.updated = false;

						break;
						break;
					}
				}

				return true;
			}

			void OnObjectReleased(const NzRefCounted* resource, int index) override
			{
				OnObjectDestroy(resource, index);
			}
	};

	ObjectListener listener;

	void Skin_MonoCPU(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton, NzVertexBuffer* buffer)
	{
		NzBufferMapper<NzVertexBuffer> inputMapper(mesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
		NzBufferMapper<NzVertexBuffer> outputMapper(buffer, nzBufferAccess_DiscardAndWrite);

		NzSkinningData skinningData;
		skinningData.inputVertex = static_cast<NzSkeletalMeshVertex*>(inputMapper.GetPointer());
		skinningData.outputVertex = static_cast<NzMeshVertex*>(outputMapper.GetPointer());
		skinningData.joints = skeleton->GetJoints();

		NzSkinPositionNormalTangent(skinningData, 0, mesh->GetVertexCount());
	}

	void Skin_MultiCPU(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton, NzVertexBuffer* buffer)
	{
		NzBufferMapper<NzVertexBuffer> inputMapper(mesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
		NzBufferMapper<NzVertexBuffer> outputMapper(buffer, nzBufferAccess_DiscardAndWrite);

		NzSkinningData skinningData;
		skinningData.inputVertex = static_cast<NzSkeletalMeshVertex*>(inputMapper.GetPointer());
		skinningData.outputVertex = static_cast<NzMeshVertex*>(outputMapper.GetPointer());
		skinningData.joints = skeleton->GetJoints();

		// Afin d'empêcher les différents threads de vouloir mettre à jour la même matrice en même temps,
		// on se charge de la mettre à jour avant de les lancer
		unsigned int jointCount = skeleton->GetJointCount();
		for (unsigned int i = 0; i < jointCount; ++i)
			skinningData.joints[i].EnsureSkinningMatrixUpdate();

		unsigned int workerCount = NzTaskScheduler::GetWorkerCount();

		std::ldiv_t div = std::ldiv(mesh->GetVertexCount(), workerCount);
		for (unsigned int i = 0; i < workerCount; ++i)
			NzTaskScheduler::AddTask(NzSkinPositionNormalTangent, skinningData, i*div.quot, (i == workerCount-1) ? div.quot + div.rem : div.quot);

		NzTaskScheduler::Run();
		NzTaskScheduler::WaitForTasks();
	}
}

NzVertexBuffer* NzSkinningManager::GetBuffer(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!mesh)
	{
		NazaraError("Invalid mesh");
		return nullptr;
	}

	if (!skeleton)
	{
		NazaraError("Invalid skeleton");
		return nullptr;
	}
	#endif

	NzErrorFlags flags(nzErrorFlag_ThrowException);

	SkeletonMap::iterator it = s_cache.find(skeleton);
	if (it == s_cache.end())
		it = s_cache.insert(std::make_pair(skeleton, std::make_pair(NzSkeletonConstListener(&listener, ObjectType_Skeleton, skeleton), MeshMap{}))).first;

	NzVertexBuffer* buffer;

    MeshMap& meshMap = it->second.second;
    MeshMap::iterator it2 = meshMap.find(mesh);
    if (it2 == meshMap.end())
	{
		NzVertexBufferRef vertexBuffer = NzVertexBuffer::New(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent), mesh->GetVertexCount(), nzDataStorage_Hardware, nzBufferUsage_Dynamic);

		BufferData data({vertexBuffer, true});
		meshMap.insert(std::make_pair(mesh, std::make_pair(NzSkeletalMeshConstListener(&listener, ObjectType_SkeletalMesh, mesh), data)));

		s_skinningQueue.push_back(SkinningData{mesh, skeleton, vertexBuffer});

		buffer = vertexBuffer;
	}
	else
	{
		BufferData& data = it2->second.second;
		if (!data.updated)
		{
			s_skinningQueue.push_back(SkinningData{mesh, skeleton, data.buffer});
			data.updated = true;
		}

		buffer = data.buffer;
	}

	return buffer;
}

void NzSkinningManager::Skin()
{
	for (SkinningData& data : s_skinningQueue)
		s_skinFunc(data.mesh, data.skeleton, data.buffer);

	s_skinningQueue.clear();
}

bool NzSkinningManager::Initialize()
{
	///TODO: GPU Skinning
	if (NzTaskScheduler::Initialize())
		s_skinFunc = Skin_MultiCPU;
	else
		s_skinFunc = Skin_MonoCPU;

	return true; // Rien de particulier à faire
}

void NzSkinningManager::Uninitialize()
{
	s_cache.clear();
	s_skinningQueue.clear();
}

NzSkinningManager::SkinFunction NzSkinningManager::s_skinFunc = nullptr;
