// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/ResourceListener.hpp>
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
	enum ResourceType
	{
		ResourceType_SkeletalMesh,
		ResourceType_Skeleton,
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

	using MeshMap = std::unordered_map<const NzSkeletalMesh*, BufferData>;
	using SkeletonMap = std::unordered_map<const NzSkeleton*, MeshMap>;
	SkeletonMap s_cache;
	std::vector<SkinningData> s_skinningQueue;

	class ResourceListener : public NzResourceListener
	{
		public:
			bool OnResourceDestroy(const NzResource* resource, int index)
			{
				switch (index)
				{
					case ResourceType_SkeletalMesh:
					{
						for (auto& pair : s_cache)
						{
							MeshMap& meshMap = pair.second;
							meshMap.erase(static_cast<const NzSkeletalMesh*>(resource));
						}
						break;
					}

					case ResourceType_Skeleton:
						s_cache.erase(static_cast<const NzSkeleton*>(resource));
						break;
				}

				return false;
			}

			bool OnResourceModified(const NzResource* resource, int index, unsigned int code)
			{
				NazaraUnused(code);

				switch (index)
				{
					case ResourceType_SkeletalMesh:
					{
						for (auto& pair : s_cache)
						{
							MeshMap& meshMap = pair.second;
							for (auto& pair2 : meshMap)
								pair2.second.updated = false;
						}
						break;
					}

					case ResourceType_Skeleton:
					{
						for (auto& pair : s_cache.at(static_cast<const NzSkeleton*>(resource)))
							pair.second.updated = false;
						break;
					}
				}

				return true;
			}

			void OnResourceReleased(const NzResource* resource, int index)
			{
				OnResourceDestroy(resource, index);
			}
	};

	ResourceListener listener;

	void Skin_MonoCPU(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton, NzVertexBuffer* buffer)
	{
		NzBufferMapper<NzVertexBuffer> mapper(buffer, nzBufferAccess_DiscardAndWrite);

		NzSkinningData skinningData;
		skinningData.inputVertex = mesh->GetBindPoseBuffer();
		skinningData.outputVertex = static_cast<NzMeshVertex*>(mapper.GetPointer());
		skinningData.joints = skeleton->GetJoints();
		skinningData.vertexWeights = mesh->GetVertexWeight(0);
		skinningData.weights = mesh->GetWeight(0);

		NzSkinPositionNormalTangent(skinningData, 0, mesh->GetVertexCount());
	}

	void Skin_MultiCPU(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton, NzVertexBuffer* buffer)
	{
		NzBufferMapper<NzVertexBuffer> mapper(buffer, nzBufferAccess_DiscardAndWrite);

		NzSkinningData skinningData;
		skinningData.inputVertex = mesh->GetBindPoseBuffer();
		skinningData.outputVertex = static_cast<NzMeshVertex*>(mapper.GetPointer());
		skinningData.joints = skeleton->GetJoints();
		skinningData.vertexWeights = mesh->GetVertexWeight(0);
		skinningData.weights = mesh->GetWeight(0);

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
	{
		it = s_cache.insert(std::make_pair(skeleton, SkeletonMap::mapped_type())).first;
		skeleton->AddResourceListener(&listener, ResourceType_Skeleton);
	}

	NzVertexBuffer* buffer;

    MeshMap& meshMap = it->second;
    MeshMap::iterator it2 = meshMap.find(mesh);
    if (it2 == meshMap.end())
	{
		std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer);
		vertexBuffer->SetPersistent(false);
		vertexBuffer->Reset(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent), mesh->GetVertexCount(), nzBufferStorage_Hardware, nzBufferUsage_Dynamic);

		BufferData data({vertexBuffer.get(), true});
		meshMap.insert(std::make_pair(mesh, data));

		mesh->AddResourceListener(&listener, ResourceType_SkeletalMesh);

		s_skinningQueue.push_back(SkinningData{mesh, skeleton, vertexBuffer.get()});

		buffer = vertexBuffer.release();
	}
	else
	{
		BufferData& data = it2->second;
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
	for (auto& pair : s_cache)
	{
		pair.first->RemoveResourceListener(&listener);
		MeshMap& meshMap = pair.second;
		for (auto& pair2 : meshMap)
			pair2.first->RemoveResourceListener(&listener);
	}
	s_cache.clear();
	s_skinningQueue.clear();
}

NzSkinningManager::SkinFunction NzSkinningManager::s_skinFunc = nullptr;
