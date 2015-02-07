// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

bool NzSkeletalModelParameters::IsValid() const
{
	if (!NzModelParameters::IsValid())
		return false;

	if (loadAnimation && !animation.IsValid())
		return false;

	return true;
}

NzSkeletalModel::NzSkeletalModel() :
m_currentSequence(nullptr),
m_animationEnabled(true)
{
}

NzSkeletalModel::NzSkeletalModel(const NzSkeletalModel& model) :
NzModel(model),
m_skeleton(model.m_skeleton),
m_currentSequence(model.m_currentSequence),
m_animationEnabled(model.m_animationEnabled),
m_interpolation(model.m_interpolation),
m_currentFrame(model.m_currentFrame),
m_nextFrame(model.m_nextFrame)
{
}

NzSkeletalModel::~NzSkeletalModel()
{
	if (m_scene)
		m_scene->UnregisterForUpdate(this);
}

void NzSkeletalModel::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	const NzMatrix4f& transformMatrix = GetTransformMatrix();

	unsigned int submeshCount = m_mesh->GetSubMeshCount();
	for (unsigned int i = 0; i < submeshCount; ++i)
	{
		const NzSkeletalMesh* mesh = static_cast<const NzSkeletalMesh*>(m_mesh->GetSubMesh(i));
		const NzMaterial* material = m_materials[mesh->GetMaterialIndex()];

		NzMeshData meshData;
		meshData.indexBuffer = mesh->GetIndexBuffer();
		meshData.primitiveMode = mesh->GetPrimitiveMode();
		meshData.vertexBuffer = NzSkinningManager::GetBuffer(mesh, &m_skeleton);

		renderQueue->AddMesh(material, meshData, m_skeleton.GetAABB(), transformMatrix);
	}
}

void NzSkeletalModel::AdvanceAnimation(float elapsedTime)
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

NzSkeletalModel* NzSkeletalModel::Clone() const
{
	return new NzSkeletalModel(*this);
}

NzSkeletalModel* NzSkeletalModel::Create() const
{
	return new NzSkeletalModel;
}

void NzSkeletalModel::EnableAnimation(bool animation)
{
	m_animationEnabled = animation;
}

NzAnimation* NzSkeletalModel::GetAnimation() const
{
	return m_animation;
}

NzSkeleton* NzSkeletalModel::GetSkeleton()
{
	InvalidateBoundingVolume();

	return &m_skeleton;
}

const NzSkeleton* NzSkeletalModel::GetSkeleton() const
{
	return &m_skeleton;
}

bool NzSkeletalModel::HasAnimation() const
{
	return m_animation != nullptr;
}

bool NzSkeletalModel::IsAnimated() const
{
	return true;
}

bool NzSkeletalModel::IsAnimationEnabled() const
{
	return m_animationEnabled;
}

bool NzSkeletalModel::IsDrawable() const
{
	return m_mesh != nullptr && m_mesh->GetSubMeshCount() >= 1;
}

bool NzSkeletalModel::LoadFromFile(const NzString& filePath, const NzSkeletalModelParameters& params)
{
	return NzSkeletalModelLoader::LoadFromFile(this, filePath, params);
}

bool NzSkeletalModel::LoadFromMemory(const void* data, std::size_t size, const NzSkeletalModelParameters& params)
{
	return NzSkeletalModelLoader::LoadFromMemory(this, data, size, params);
}

bool NzSkeletalModel::LoadFromStream(NzInputStream& stream, const NzSkeletalModelParameters& params)
{
	return NzSkeletalModelLoader::LoadFromStream(this, stream, params);
}

void NzSkeletalModel::Reset()
{
	NzModel::Reset();

	m_skeleton.Destroy();
}

bool NzSkeletalModel::SetAnimation(NzAnimation* animation)
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

void NzSkeletalModel::SetMesh(NzMesh* mesh)
{
	#if NAZARA_GRAPHICS_SAFE
	if (mesh && mesh->GetAnimationType() != nzAnimationType_Skeletal)
	{
		NazaraError("Mesh animation type must be skeletal");
		return;
	}
	#endif

	NzModel::SetMesh(mesh);

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

bool NzSkeletalModel::SetSequence(const NzString& sequenceName)
{
	///TODO: Rendre cette erreur "safe" avec le nouveau système de gestions d'erreur (No-log)
	#if NAZARA_GRAPHICS_SAFE
	if (!m_animation)
	{
		NazaraError("Model has no animation");
		return false;
	}
	#endif

	const NzSequence* currentSequence = m_animation->GetSequence(sequenceName);
	if (!currentSequence)
	{
		NazaraError("Sequence not found");
		return false;
	}

	m_currentSequence = currentSequence;
	m_nextFrame = m_currentSequence->firstFrame;

	return true;
}

void NzSkeletalModel::SetSequence(unsigned int sequenceIndex)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_animation)
	{
		NazaraError("Model has no animation");
		return;
	}
	#endif

	const NzSequence* currentSequence = m_animation->GetSequence(sequenceIndex);
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

NzSkeletalModel& NzSkeletalModel::operator=(const NzSkeletalModel& node)
{
	NzSkeletalModel::operator=(node);

	m_animation = node.m_animation;
	m_animationEnabled = node.m_animationEnabled;
	m_currentFrame = node.m_currentFrame;
	m_currentSequence = node.m_currentSequence;
	m_interpolation = node.m_interpolation;
	m_nextFrame = node.m_nextFrame;
	m_skeleton = node.m_skeleton;

	return *this;
}

NzSkeletalModel& NzSkeletalModel::operator=(NzSkeletalModel&& node)
{
	NzModel::operator=(node);

	// Ressources
	m_animation = std::move(node.m_animation);
	m_skeleton = std::move(node.m_skeleton);

	// Paramètres
	m_animationEnabled = node.m_animationEnabled;
	m_currentFrame = node.m_currentFrame;
	m_currentSequence = node.m_currentSequence;
	m_interpolation = node.m_interpolation;
	m_nextFrame = node.m_nextFrame;

	return *this;
}

void NzSkeletalModel::MakeBoundingVolume() const
{
	m_boundingVolume.Set(m_skeleton.GetAABB());
}

void NzSkeletalModel::Register()
{
	m_scene->RegisterForUpdate(this);
}

void NzSkeletalModel::Unregister()
{
	m_scene->UnregisterForUpdate(this);
}

void NzSkeletalModel::Update()
{
	if (m_animationEnabled && m_animation)
		AdvanceAnimation(m_scene->GetUpdateTime());
}

NzSkeletalModelLoader::LoaderList NzSkeletalModel::s_loaders;
