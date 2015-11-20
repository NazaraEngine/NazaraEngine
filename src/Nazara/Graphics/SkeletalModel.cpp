// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	bool SkeletalModelParameters::IsValid() const
	{
		if (!ModelParameters::IsValid())
			return false;

		if (loadAnimation && !animation.IsValid())
			return false;

		return true;
	}

	SkeletalModel::SkeletalModel() :
	m_currentSequence(nullptr),
	m_animationEnabled(true)
	{
	}

	void SkeletalModel::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		if (!m_mesh)
			return;

		unsigned int submeshCount = m_mesh->GetSubMeshCount();
		for (unsigned int i = 0; i < submeshCount; ++i)
		{
			const SkeletalMesh* mesh = static_cast<const SkeletalMesh*>(m_mesh->GetSubMesh(i));
			const Material* material = m_materials[mesh->GetMaterialIndex()];

			MeshData meshData;
			meshData.indexBuffer = mesh->GetIndexBuffer();
			meshData.primitiveMode = mesh->GetPrimitiveMode();
			meshData.vertexBuffer = SkinningManager::GetBuffer(mesh, &m_skeleton);

			renderQueue->AddMesh(material, meshData, m_skeleton.GetAABB(), instanceData.transformMatrix);
		}
	}

	void SkeletalModel::AdvanceAnimation(float elapsedTime)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!m_animation)
		{
			NazaraError("Model has no animation");
			return;
		}
		#endif

		m_interpolation += m_currentSequence->frameRate * elapsedTime;
		while (m_interpolation > 1.f)
		{
			m_interpolation -= 1.f;

			unsigned lastFrame = m_currentSequence->firstFrame + m_currentSequence->frameCount - 1;
			if (m_nextFrame+1 > lastFrame)
			{
				if (m_animation->IsLoopPointInterpolationEnabled())
				{
					m_currentFrame = m_nextFrame;
					m_nextFrame = m_currentSequence->firstFrame;
				}
				else
				{
					m_currentFrame = m_currentSequence->firstFrame;
					m_nextFrame = m_currentFrame+1;
				}
			}
			else
			{
				m_currentFrame = m_nextFrame;
				m_nextFrame++;
			}
		}

		m_animation->AnimateSkeleton(&m_skeleton, m_currentFrame, m_nextFrame, m_interpolation);

		InvalidateBoundingVolume();
	}

	SkeletalModel* SkeletalModel::Clone() const
	{
		return new SkeletalModel(*this);
	}

	SkeletalModel* SkeletalModel::Create() const
	{
		return new SkeletalModel;
	}

	void SkeletalModel::EnableAnimation(bool animation)
	{
		m_animationEnabled = animation;
	}

	Animation* SkeletalModel::GetAnimation() const
	{
		return m_animation;
	}

	Skeleton* SkeletalModel::GetSkeleton()
	{
		InvalidateBoundingVolume();

		return &m_skeleton;
	}

	const Skeleton* SkeletalModel::GetSkeleton() const
	{
		return &m_skeleton;
	}

	bool SkeletalModel::HasAnimation() const
	{
		return m_animation != nullptr;
	}

	bool SkeletalModel::IsAnimated() const
	{
		return true;
	}

	bool SkeletalModel::IsAnimationEnabled() const
	{
		return m_animationEnabled;
	}

	bool SkeletalModel::LoadFromFile(const String& filePath, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromFile(this, filePath, params);
	}

	bool SkeletalModel::LoadFromMemory(const void* data, std::size_t size, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromMemory(this, data, size, params);
	}

	bool SkeletalModel::LoadFromStream(Stream& stream, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromStream(this, stream, params);
	}

	void SkeletalModel::Reset()
	{
		Model::Reset();

		m_skeleton.Destroy();
	}

	bool SkeletalModel::SetAnimation(Animation* animation)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!m_mesh)
		{
			NazaraError("Model has no mesh");
			return false;
		}

		if (animation)
		{
			if (!animation->IsValid())
			{
				NazaraError("Invalid animation");
				return false;
			}

			if (animation->GetType() != m_mesh->GetAnimationType())
			{
				NazaraError("Animation type must match mesh animation type");
				return false;
			}

			if (animation->GetJointCount() != m_mesh->GetJointCount())
			{
				NazaraError("Animation joint count must match mesh joint count");
				return false;
			}
		}
		#endif

		m_animation = animation;
		if (m_animation)
		{
			m_currentFrame = 0;
			m_interpolation = 0.f;

			SetSequence(0);
		}

		return true;
	}

	void SkeletalModel::SetMesh(Mesh* mesh)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (mesh && mesh->GetAnimationType() != AnimationType_Skeletal)
		{
			NazaraError("Mesh animation type must be skeletal");
			return;
		}
		#endif

		Model::SetMesh(mesh);

		if (m_mesh)
		{
			if (m_animation && m_animation->GetJointCount() != m_mesh->GetJointCount())
			{
				NazaraWarning("Animation joint count is not matching new mesh joint count, disabling animation...");
				SetAnimation(nullptr);
			}

			m_skeleton = *m_mesh->GetSkeleton(); // Copie du squelette template
		}
	}

	bool SkeletalModel::SetSequence(const String& sequenceName)
	{
		///TODO: Rendre cette erreur "safe" avec le nouveau système de gestions d'erreur (No-log)
		#if NAZARA_GRAPHICS_SAFE
		if (!m_animation)
		{
			NazaraError("Model has no animation");
			return false;
		}
		#endif

		const Sequence* currentSequence = m_animation->GetSequence(sequenceName);
		if (!currentSequence)
		{
			NazaraError("Sequence not found");
			return false;
		}

		m_currentSequence = currentSequence;
		m_nextFrame = m_currentSequence->firstFrame;

		return true;
	}

	void SkeletalModel::SetSequence(unsigned int sequenceIndex)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!m_animation)
		{
			NazaraError("Model has no animation");
			return;
		}
		#endif

		const Sequence* currentSequence = m_animation->GetSequence(sequenceIndex);
		#if NAZARA_GRAPHICS_SAFE
		if (!currentSequence)
		{
			NazaraError("Sequence not found");
			return;
		}
		#endif

		m_currentSequence = currentSequence;
		m_nextFrame = m_currentSequence->firstFrame;
	}

	void SkeletalModel::MakeBoundingVolume() const
	{
		m_boundingVolume.Set(m_skeleton.GetAABB());
	}

	void SkeletalModel::Update()
	{
		/*if (m_animationEnabled && m_animation)
			AdvanceAnimation(m_scene->GetUpdateTime());*/
	}

	SkeletalModelLoader::LoaderList SkeletalModel::s_loaders;
}
