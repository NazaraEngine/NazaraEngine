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
	/*!
	* \ingroup graphics
	* \class Nz::SkeletalModel
	* \brief Graphics class that represents a model with a skeleton
	*/

	/*!
	* \brief Checks whether the parameters for the skeletal mesh are correct
	* \return true If parameters are valid
	*/

	bool SkeletalModelParameters::IsValid() const
	{
		if (!ModelParameters::IsValid())
			return false;

		if (loadAnimation && !animation.IsValid())
			return false;

		return true;
	}

	/*!
	* \brief Constructs a SkeletalModel object by default
	*/

	SkeletalModel::SkeletalModel() :
	m_currentSequence(nullptr),
	m_animationEnabled(true)
	{
	}

	/*!
	* \brief Adds the skeletal mesh to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/

	void SkeletalModel::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		if (!m_mesh)
			return;

		unsigned int submeshCount = m_mesh->GetSubMeshCount();
		for (unsigned int i = 0; i < submeshCount; ++i)
		{
			if (!m_enabledSubmeshes.Test(i))
				continue;

			const SkeletalMesh* mesh = static_cast<const SkeletalMesh*>(m_mesh->GetSubMesh(i));
			const Material* material = m_materials[mesh->GetMaterialIndex()];

			MeshData meshData;
			meshData.indexBuffer = mesh->GetIndexBuffer();
			meshData.primitiveMode = mesh->GetPrimitiveMode();
			meshData.vertexBuffer = SkinningManager::GetBuffer(mesh, &m_skeleton);

			renderQueue->AddMesh(instanceData.renderOrder, material, meshData, m_skeleton.GetAABB(), instanceData.transformMatrix);
		}
	}

	/*!
	* \brief Updates the animation of the mesh
	*
	* \param elapsedTime Delta time between two frames
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if there is no animation
	*/

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
			if (m_nextFrame + 1 > lastFrame)
			{
				if (m_animation->IsLoopPointInterpolationEnabled())
				{
					m_currentFrame = m_nextFrame;
					m_nextFrame = m_currentSequence->firstFrame;
				}
				else
				{
					m_currentFrame = m_currentSequence->firstFrame;
					m_nextFrame = m_currentFrame + 1;
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

	/*!
	* \brief Clones this skeletal model
	* \return Pointer to newly allocated SkeletalModel
	*/

	SkeletalModel* SkeletalModel::Clone() const
	{
		return new SkeletalModel(*this);
	}

	/*!
	* \brief Creates a default skeletal model
	* \return Pointer to newly allocated SkeletalModel
	*/

	SkeletalModel* SkeletalModel::Create() const
	{
		return new SkeletalModel;
	}

	/*!
	* \brief Enables the animation of the model
	*
	* \param animation Should the model be animated
	*/

	void SkeletalModel::EnableAnimation(bool animation)
	{
		m_animationEnabled = animation;
	}

	/*!
	* \brief Gets the animation of the model
	* \return Pointer to the animation
	*/

	Animation* SkeletalModel::GetAnimation() const
	{
		return m_animation;
	}

	/*!
	* \brief Gets the skeleton of the model
	* \return Pointer to the skeleton
	*/

	Skeleton* SkeletalModel::GetSkeleton()
	{
		InvalidateBoundingVolume();

		return &m_skeleton;
	}

	/*!
	* \brief Gets the skeleton of the model
	* \return Constant pointer to the skeleton
	*/

	const Skeleton* SkeletalModel::GetSkeleton() const
	{
		return &m_skeleton;
	}

	/*!
	* \brief Checks whether the skeleton has an animation
	* \return true If it is the case
	*
	* \see IsAnimated, IsAnimationEnabled
	*/

	bool SkeletalModel::HasAnimation() const
	{
		return m_animation != nullptr;
	}

	/*!
	* \brief Checks whether the skeleton is animated
	* \return true
	*
	* \see HasAnimation, IsAnimationEnabled
	*/

	bool SkeletalModel::IsAnimated() const
	{
		return true;
	}

	/*!
	* \brief Checks whether the skeleton is currently animated
	* \return true If it is the case
	*
	* \see HasAnimation, IsAnimated
	*/

	bool SkeletalModel::IsAnimationEnabled() const
	{
		return m_animationEnabled;
	}

	/*!
	* \brief Loads the skeleton model from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the skeleton model
	*/

	bool SkeletalModel::LoadFromFile(const String& filePath, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromFile(this, filePath, params);
	}

	/*!
	* \brief Loads the skeleton model from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the skeleton model
	*/

	bool SkeletalModel::LoadFromMemory(const void* data, std::size_t size, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromMemory(this, data, size, params);
	}

	/*!
	* \brief Loads the skeleton model from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the skeleton model
	* \param params Parameters for the skeleton model
	*/

	bool SkeletalModel::LoadFromStream(Stream& stream, const SkeletalModelParameters& params)
	{
		return SkeletalModelLoader::LoadFromStream(this, stream, params);
	}

	/*!
	* \brief Resets the model
	*/

	void SkeletalModel::Reset()
	{
		Model::Reset();

		m_skeleton.Destroy();
	}

	/*!
	* \brief Sets the animation for the model
	* \return true If successful
	*
	* \param animation Animation for the model
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if there is no mesh
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if animation is invalid
	*/

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

	/*!
	* \brief Sets the mesh for the model
	*
	* \param mesh Mesh for the model
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if there is no mesh or if invalid
	*/

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

			m_skeleton = *m_mesh->GetSkeleton(); // Copy of skeleton template
		}
	}

	/*!
	* \brief Sets the sequence for the model
	* \return true If successful
	*
	* \param sequenceName Name for the sequence animation
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if there is no animation
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if sequence name does not exist for the current animation
	*/

	bool SkeletalModel::SetSequence(const String& sequenceName)
	{
		///TODO: Make this error "safe" with the new system of error handling (No-log)
		#if NAZARA_GRAPHICS_SAFE
		if (!m_animation)
		{
			NazaraError("Model has no animation");
			return false;
		}
		#endif

		const Sequence* currentSequence = m_animation->GetSequence(sequenceName);
		#if NAZARA_GRAPHICS_SAFE
		if (!currentSequence)
		{
			NazaraError("Sequence not found");
			return false;
		}
		#endif

		m_currentSequence = currentSequence;
		m_nextFrame = m_currentSequence->firstFrame;

		return true;
	}

	/*!
	* \brief Sets the sequence for the model
	*
	* \param sequenceIndex Index for the sequence animation
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if there is no animation
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if sequence name does not exist for the current animation
	*/

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

	/*
	* \brief Makes the bounding volume of this text
	*/

	void SkeletalModel::MakeBoundingVolume() const
	{
		m_boundingVolume.Set(m_skeleton.GetAABB());
	}

	/*!
	* \brief Updates the model
	*/

	void SkeletalModel::Update()
	{
		///TODO
		/*if (m_animationEnabled && m_animation)
			AdvanceAnimation(m_scene->GetUpdateTime());*/
	}

	SkeletalModelLoader::LoaderList SkeletalModel::s_loaders;
}
