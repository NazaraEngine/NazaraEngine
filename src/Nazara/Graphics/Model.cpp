// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

bool NzModelParameters::IsValid() const
{
	if (loadAnimation && !animation.IsValid())
		return false;

	if (loadMaterials && !material.IsValid())
		return false;

	return mesh.IsValid();
}

NzModel::NzModel() :
m_currentSequence(nullptr),
m_animationEnabled(true),
m_boundingVolumeUpdated(true),
m_matCount(0),
m_skin(0),
m_skinCount(1)
{
}

NzModel::NzModel(const NzModel& model) :
NzSceneNode(model),
m_materials(model.m_materials),
m_boundingVolume(model.m_boundingVolume),
m_currentSequence(model.m_currentSequence),
m_animationEnabled(model.m_animationEnabled),
m_boundingVolumeUpdated(model.m_boundingVolumeUpdated),
m_interpolation(model.m_interpolation),
m_currentFrame(model.m_currentFrame),
m_matCount(model.m_matCount),
m_nextFrame(model.m_nextFrame),
m_skin(model.m_skin),
m_skinCount(model.m_skinCount)
{
	if (model.m_mesh)
	{
		// Nous n'avons une animation et des matériaux que si nous avons un mesh
		m_animation = model.m_animation;
		m_mesh = model.m_mesh;
		m_materials = model.m_materials;

		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_skeleton = model.m_skeleton;
	}
}

NzModel::~NzModel()
{
	Reset();
}

void NzModel::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	renderQueue->AddModel(this);
}

void NzModel::AdvanceAnimation(float elapsedTime)
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
	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
}

void NzModel::EnableAnimation(bool animation)
{
	m_animationEnabled = animation;
}

NzAnimation* NzModel::GetAnimation() const
{
	return m_animation;
}

const NzBoundingVolumef& NzModel::GetBoundingVolume() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no mesh");

		static NzBoundingVolumef dummy(nzExtend_Null);
		return dummy;
	}
	#endif

	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return m_boundingVolume;
}

NzMaterial* NzModel::GetMaterial(const NzString& subMeshName) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no mesh");
		return nullptr;
	}
	#endif

	NzSubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
	if (!subMesh)
	{
		NazaraError("Mesh has no submesh \"" + subMeshName + '"');
		return nullptr;
	}

	unsigned int matIndex = subMesh->GetMaterialIndex();
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount) + ')');
		return nullptr;
	}

	return m_materials[m_skin*m_matCount + matIndex];
}

NzMaterial* NzModel::GetMaterial(unsigned int matIndex) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount) + ')');
		return nullptr;
	}
	#endif

	return m_materials[m_skin*m_matCount + matIndex];
}

NzMaterial* NzModel::GetMaterial(unsigned int skinIndex, const NzString& subMeshName) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (skinIndex >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skinIndex) + " >= " + NzString::Number(m_skinCount) + ')');
		return nullptr;
	}
	#endif

	NzSubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
	if (!subMesh)
	{
		NazaraError("Mesh has no submesh \"" + subMeshName + '"');
		return nullptr;
	}

	unsigned int matIndex = subMesh->GetMaterialIndex();
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount) + ')');
		return nullptr;
	}

	return m_materials[skinIndex*m_matCount + matIndex];
}

NzMaterial* NzModel::GetMaterial(unsigned int skinIndex, unsigned int matIndex) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (skinIndex >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skinIndex) + " >= " + NzString::Number(m_skinCount) + ')');
		return nullptr;
	}

	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount) + ')');
		return nullptr;
	}
	#endif

	return m_materials[skinIndex*m_matCount + matIndex];
}

unsigned int NzModel::GetMaterialCount() const
{
	return m_matCount;
}

NzMesh* NzModel::GetMesh() const
{
	return m_mesh;
}

nzSceneNodeType NzModel::GetSceneNodeType() const
{
	return nzSceneNodeType_Model;
}

NzSkeleton* NzModel::GetSkeleton()
{
	return &m_skeleton;
}

const NzSkeleton* NzModel::GetSkeleton() const
{
	return &m_skeleton;
}

unsigned int NzModel::GetSkin() const
{
	return m_skin;
}

unsigned int NzModel::GetSkinCount() const
{
	return m_skinCount;
}

bool NzModel::HasAnimation() const
{
	return m_animation != nullptr;
}

bool NzModel::IsAnimationEnabled() const
{
	return m_animationEnabled;
}

bool NzModel::IsDrawable() const
{
	return m_mesh != nullptr && m_mesh->GetSubMeshCount() >= 1;
}

void NzModel::InvalidateBoundingVolume()
{
	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
}

bool NzModel::LoadFromFile(const NzString& filePath, const NzModelParameters& params)
{
	return NzModelLoader::LoadFromFile(this, filePath, params);
}

bool NzModel::LoadFromMemory(const void* data, std::size_t size, const NzModelParameters& params)
{
	return NzModelLoader::LoadFromMemory(this, data, size, params);
}

bool NzModel::LoadFromStream(NzInputStream& stream, const NzModelParameters& params)
{
	return NzModelLoader::LoadFromStream(this, stream, params);
}

void NzModel::Reset()
{
	if (m_scene)
		m_scene->UnregisterForUpdate(this);

	m_matCount = 0;
	m_skinCount = 0;

	if (m_mesh)
	{
		m_animation.Reset();
		m_mesh.Reset();
		m_materials.clear();

		m_skeleton.Destroy();
	}
}

bool NzModel::SetAnimation(NzAnimation* animation)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no animation");
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

		if (animation->GetType() == nzAnimationType_Skeletal && animation->GetJointCount() != m_mesh->GetJointCount())
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

		if (m_scene)
			m_scene->RegisterForUpdate(this);
	}
	else if (m_scene)
		m_scene->UnregisterForUpdate(this);

	return true;
}

bool NzModel::SetMaterial(const NzString& subMeshName, NzMaterial* material)
{
	NzSubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
	if (!subMesh)
	{
		NazaraError("Mesh has no submesh \"" + subMeshName + '"');
		return false;
	}

	unsigned int matIndex = subMesh->GetMaterialIndex();
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount) + ')');
		return false;
	}

	unsigned int index = m_skin*m_matCount + matIndex;

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();

	return true;
}

void NzModel::SetMaterial(unsigned int matIndex, NzMaterial* material)
{
	#if NAZARA_GRAPHICS_SAFE
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return;
	}
	#endif

	unsigned int index = m_skin*m_matCount + matIndex;

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();
}

bool NzModel::SetMaterial(unsigned int skinIndex, const NzString& subMeshName, NzMaterial* material)
{
	#if NAZARA_GRAPHICS_SAFE
	if (skinIndex >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skinIndex) + " >= " + NzString::Number(m_skinCount));
		return false;
	}
	#endif

	NzSubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
	if (!subMesh)
	{
		NazaraError("Mesh has no submesh \"" + subMeshName + '"');
		return false;
	}

	unsigned int matIndex = subMesh->GetMaterialIndex();
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return false;
	}

	unsigned int index = skinIndex*m_matCount + matIndex;

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();

	return true;
}

void NzModel::SetMaterial(unsigned int skinIndex, unsigned int matIndex, NzMaterial* material)
{
	#if NAZARA_GRAPHICS_SAFE
	if (skinIndex >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skinIndex) + " >= " + NzString::Number(m_skinCount));
		return;
	}

	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return;
	}
	#endif

	unsigned int index = skinIndex*m_matCount + matIndex;

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();
}

void NzModel::SetMesh(NzMesh* mesh)
{
	m_mesh = mesh;

	if (m_mesh)
	{
		m_boundingVolume.MakeNull();
		m_boundingVolumeUpdated = false;

		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_skeleton = *mesh->GetSkeleton(); // Copie du squelette template

		if (m_animation)
		{
			if (m_animation->GetJointCount() != m_mesh->GetJointCount())
			{
				NazaraWarning("Animation joint count is not matching new mesh joint count");
				SetAnimation(nullptr);
			}
		}

		m_matCount = mesh->GetMaterialCount();
		m_materials.clear();
		m_materials.resize(m_matCount, NzMaterial::GetDefault());
		m_skinCount = 1;
	}
	else
	{
		m_boundingVolume.MakeNull();
		m_boundingVolumeUpdated = true;
		m_matCount = 0;
		m_materials.clear();
		m_skinCount = 0;

		SetAnimation(nullptr);
	}
}

bool NzModel::SetSequence(const NzString& sequenceName)
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

void NzModel::SetSequence(unsigned int sequenceIndex)
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

void NzModel::SetSkin(unsigned int skin)
{
	#if NAZARA_GRAPHICS_SAFE
	if (skin >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skin) + " >= " + NzString::Number(m_skinCount) + ')');
		return;
	}
	#endif

	m_skin = skin;
}

void NzModel::SetSkinCount(unsigned int skinCount)
{
	#if NAZARA_GRAPHICS_SAFE
	if (skinCount == 0)
	{
		NazaraError("Skin count must be over 0");
		return;
	}
	#endif

	m_materials.resize(m_matCount*skinCount, NzMaterial::GetDefault());
	m_skinCount = skinCount;
}

NzModel& NzModel::operator=(const NzModel& node)
{
	NzSceneNode::operator=(node);

	m_animation = node.m_animation;
	m_animationEnabled = node.m_animationEnabled;
	m_boundingVolume = node.m_boundingVolume;
	m_boundingVolumeUpdated = node.m_boundingVolumeUpdated;
	m_currentFrame = node.m_currentFrame;
	m_currentSequence = node.m_currentSequence;
	m_interpolation = node.m_interpolation;
	m_matCount = node.m_matCount;
	m_materials = node.m_materials;
	m_mesh = node.m_mesh;
	m_nextFrame = node.m_nextFrame;
	m_skin = node.m_skin;
	m_skinCount = node.m_skinCount;

	if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
		m_skeleton = node.m_skeleton;

	return *this;
}

NzModel& NzModel::operator=(NzModel&& node)
{
	NzSceneNode::operator=(node);

	// Ressources
	m_animation = std::move(node.m_animation);
	m_mesh = std::move(node.m_mesh);
	m_materials = std::move(node.m_materials);

	if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
		m_skeleton = std::move(node.m_skeleton);

	// Paramètres
	m_animationEnabled = node.m_animationEnabled;
	m_boundingVolume = node.m_boundingVolume;
	m_boundingVolumeUpdated = node.m_boundingVolumeUpdated;
	m_currentFrame = node.m_currentFrame;
	m_currentSequence = node.m_currentSequence;
	m_interpolation = node.m_interpolation;
	m_matCount = node.m_matCount;
	m_nextFrame = node.m_nextFrame;
	m_skin = node.m_skin;
	m_skinCount = node.m_skinCount;

	return *this;
}

bool NzModel::FrustumCull(const NzFrustumf& frustum)
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return frustum.Contains(m_boundingVolume);
}

void NzModel::Invalidate()
{
	NzSceneNode::Invalidate();

	m_boundingVolumeUpdated = false;
}

void NzModel::Register()
{
	if (m_animation)
		m_scene->RegisterForUpdate(this);
}

void NzModel::Unregister()
{
	m_scene->UnregisterForUpdate(this);
}

void NzModel::Update()
{
	if (m_animationEnabled && m_animation)
		AdvanceAnimation(m_scene->GetUpdateTime());
}

void NzModel::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
	{
		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_boundingVolume.Set(m_skeleton.GetAABB());
		else
			m_boundingVolume.Set(m_mesh->GetAABB());
	}

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingVolume.Update(m_transformMatrix);
	m_boundingVolumeUpdated = true;
}

NzModelLoader::LoaderList NzModel::s_loaders;
