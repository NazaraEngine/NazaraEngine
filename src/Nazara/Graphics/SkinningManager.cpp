// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct BufferData
		{
			NazaraSlot(SkeletalMesh, OnSkeletalMeshDestroy, skeletalMeshDestroySlot);

			VertexBufferRef buffer;
			bool updated;
		};

		using MeshMap = std::unordered_map<const SkeletalMesh*, BufferData>;

		struct MeshData
		{
			NazaraSlot(Skeleton, OnSkeletonDestroy, skeletonDestroySlot);
			NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, skeletonJointsInvalidatedSlot);

			MeshMap meshMap;
		};

		struct QueueData
		{
			const SkeletalMesh* mesh;
			const Skeleton* skeleton;
			VertexBuffer* buffer;
		};

		using SkeletonMap = std::unordered_map<const Skeleton*, MeshData>;
		SkeletonMap s_cache;
		std::vector<QueueData> s_skinningQueue;

		/*!
		* \brief Skins the mesh for a single thread context
		*
		* \param mesh Skeletal mesh to get vertex buffer from
		* \param skeleton Skeleton to consider for getting data
		* \param buffer Vertex buffer symbolizing the transition
		*/

		void Skin_MonoCPU(const SkeletalMesh* mesh, const Skeleton* skeleton, VertexBuffer* buffer)
		{
			BufferMapper<VertexBuffer> inputMapper(mesh->GetVertexBuffer(), BufferAccess_ReadOnly);
			BufferMapper<VertexBuffer> outputMapper(buffer, BufferAccess_DiscardAndWrite);

			SkinningData skinningData;
			skinningData.inputVertex = static_cast<SkeletalMeshVertex*>(inputMapper.GetPointer());
			skinningData.outputVertex = static_cast<MeshVertex*>(outputMapper.GetPointer());
			skinningData.joints = skeleton->GetJoints();

			SkinPositionNormalTangent(skinningData, 0, mesh->GetVertexCount());
		}

		/*!
		* \brief Skins the mesh for a multi-threaded context
		*
		* \param mesh Skeletal mesh to get vertex buffer from
		* \param skeleton Skeleton to consider for getting data
		* \param buffer Vertex buffer symbolizing the transition
		*/

		void Skin_MultiCPU(const SkeletalMesh* mesh, const Skeleton* skeleton, VertexBuffer* buffer)
		{
			BufferMapper<VertexBuffer> inputMapper(mesh->GetVertexBuffer(), BufferAccess_ReadOnly);
			BufferMapper<VertexBuffer> outputMapper(buffer, BufferAccess_DiscardAndWrite);

			SkinningData skinningData;
			skinningData.inputVertex = static_cast<SkeletalMeshVertex*>(inputMapper.GetPointer());
			skinningData.outputVertex = static_cast<MeshVertex*>(outputMapper.GetPointer());
			skinningData.joints = skeleton->GetJoints();

			// To avoid different threads to update the same matrix at the same time
			// We try to update them before launching the tasks
			unsigned int jointCount = skeleton->GetJointCount();
			for (unsigned int i = 0; i < jointCount; ++i)
				skinningData.joints[i].EnsureSkinningMatrixUpdate();

			unsigned int workerCount = TaskScheduler::GetWorkerCount();

			std::ldiv_t div = std::ldiv(mesh->GetVertexCount(), workerCount);
			for (unsigned int i = 0; i < workerCount; ++i)
				TaskScheduler::AddTask(SkinPositionNormalTangent, skinningData, i*div.quot, (i == workerCount-1) ? div.quot + div.rem : div.quot);

			TaskScheduler::Run();
			TaskScheduler::WaitForTasks();
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::SkinningManager
	* \brief Graphics class that represents the management of skinning
	*/

	/*!
	* \brief Gets the vertex buffer from a skeletal mesh with its skeleton
	* \return A pointer to the vertex buffer newly created
	*
	* \param mesh Skeletal mesh to get vertex buffer from
	* \param skeleton Skeleton to consider for getting data
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if mesh is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if skeleton is invalid
	*/

	VertexBuffer* SkinningManager::GetBuffer(const SkeletalMesh* mesh, const Skeleton* skeleton)
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

		ErrorFlags flags(ErrorFlag_ThrowException);

		SkeletonMap::iterator it = s_cache.find(skeleton);
		if (it == s_cache.end())
		{
			MeshData meshData;
			meshData.skeletonDestroySlot.Connect(skeleton->OnSkeletonDestroy, OnSkeletonRelease);
			meshData.skeletonJointsInvalidatedSlot.Connect(skeleton->OnSkeletonJointsInvalidated, OnSkeletonInvalidated);

			it = s_cache.insert(std::make_pair(skeleton, std::move(meshData))).first;
		}

		VertexBuffer* buffer;

		MeshMap& meshMap = it->second.meshMap;
		MeshMap::iterator it2 = meshMap.find(mesh);
		if (it2 == meshMap.end())
		{
			VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent), mesh->GetVertexCount(), DataStorage_Hardware, BufferUsage_Dynamic);

			BufferData data;
			data.skeletalMeshDestroySlot.Connect(mesh->OnSkeletalMeshDestroy, OnSkeletalMeshDestroy);
			data.buffer = vertexBuffer;
			data.updated = true;

			meshMap.insert(std::make_pair(mesh, std::move(data)));

			s_skinningQueue.push_back(QueueData{mesh, skeleton, vertexBuffer});

			buffer = vertexBuffer;
		}
		else
		{
			BufferData& data = it2->second;
			if (!data.updated)
			{
				s_skinningQueue.push_back(QueueData{mesh, skeleton, data.buffer});
				data.updated = true;
			}

			buffer = data.buffer;
		}

		return buffer;
	}

	/*!
	* \brief Skins the skeletal mesh
	*/

	void SkinningManager::Skin()
	{
		for (QueueData& data : s_skinningQueue)
			s_skinFunc(data.mesh, data.skeleton, data.buffer);

		s_skinningQueue.clear();
	}

	/*!
	* \brief Initializes the skinning librairies
	* \return true
	*/

	bool SkinningManager::Initialize()
	{
		///TODO: GPU Skinning
		if (TaskScheduler::Initialize())
			s_skinFunc = Skin_MultiCPU;
		else
			s_skinFunc = Skin_MonoCPU;

		return true; // Nothing particular to do
	}

	/*!
	* \brief Handle the destruction of a skeletal mesh
	*
	* \param mesh SkeletalMesh being destroyed
	*/

	void SkinningManager::OnSkeletalMeshDestroy(const SkeletalMesh* mesh)
	{
		for (auto& pair : s_cache)
		{
			MeshMap& meshMap = pair.second.meshMap;
			meshMap.erase(mesh);
		}
	}

	/*!
	* \brief Handle the invalidation of a skeletal mesh
	*
	* \param mesh SkeletalMesh being invalidated
	*/

	void SkinningManager::OnSkeletonInvalidated(const Skeleton* skeleton)
	{
		for (auto& pair : s_cache.at(skeleton).meshMap)
			pair.second.updated = false;
	}

	/*!
	* \brief Handle the release of a skeletal mesh
	*
	* \param skeleton SkeletalMesh being released
	*/

	void SkinningManager::OnSkeletonRelease(const Skeleton* skeleton)
	{
		s_cache.erase(skeleton);
	}

	/*!
	* \brief Uninitializes the skinning librairies
	*/

	void SkinningManager::Uninitialize()
	{
		s_cache.clear();
		s_skinningQueue.clear();
	}

	SkinningManager::SkinFunction SkinningManager::s_skinFunc = nullptr;
}
