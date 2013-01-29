// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Model.hpp>
#include <Nazara/3D/Config.hpp>
#include <memory>
#include <Nazara/3D/Debug.hpp>

NzModel::NzModel() :
m_animation(nullptr),
m_mesh(nullptr),
m_currentSequence(nullptr),
m_matCount(0),
m_skinCount(0)
{
}

NzModel::NzModel(const NzModel& model) :
m_materials(model.m_materials),
m_animation(model.m_animation),
m_mesh(model.m_mesh),
m_currentSequence(model.m_currentSequence),
m_matCount(model.m_matCount),
m_skinCount(model.m_skinCount)
{
	if (m_mesh)
	{
		if (m_animation)
			m_animation->AddResourceReference();

		m_mesh->AddResourceReference();

		// Nous n'avons des matériaux que si nous avons un mesh
		for (const NzMaterial* material : m_materials)
			material->AddResourceReference();
	}
}

NzModel::~NzModel()
{
	Reset();
}

NzAnimation* NzModel::GetAnimation() const
{
	return m_animation;
}

const NzAxisAlignedBox& NzModel::GetAABB() const
{
	#if NAZARA_3D_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no mesh");
		return NzAxisAlignedBox::Null;
	}
	#endif

	return m_mesh->GetAABB();
}

NzMaterial* NzModel::GetMaterial(unsigned int matIndex) const
{
	#if NAZARA_3D_SAFE
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return nullptr;
	}
	#endif

	return m_materials[matIndex];
}

NzMaterial* NzModel::GetMaterial(unsigned int skinIndex, unsigned int matIndex) const
{
	#if NAZARA_3D_SAFE
	if (skinIndex >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skinIndex) + " >= " + NzString::Number(m_skinCount));
		return nullptr;
	}

	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return nullptr;
	}
	#endif

	return m_materials[matIndex];
}

unsigned int NzModel::GetMaterialCount() const
{
	return m_matCount;
}

unsigned int NzModel::GetSkinCount() const
{
	return m_skinCount;
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

bool NzModel::HasAnimation() const
{
	return m_animation != nullptr;
}

bool NzModel::LoadFromFile(const NzString& meshPath, const NzMeshParams& meshParameters, const NzModelParameters& modelParameters)
{
	///TODO: ResourceManager
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromFile(meshPath, meshParameters))
	{
		NazaraError("Failed to load mesh");
		return false;
	}

	mesh->SetPersistent(false, false);
	SetMesh(mesh.release(), modelParameters);

	return true;
}

bool NzModel::LoadFromMemory(const void* data, std::size_t size, const NzMeshParams& meshParameters, const NzModelParameters& modelParameters)
{
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromMemory(data, size, meshParameters))
	{
		NazaraError("Failed to load mesh");
		return false;
	}

	mesh->SetPersistent(false, false);
	SetMesh(mesh.release(), modelParameters);

	return true;
}

bool NzModel::LoadFromStream(NzInputStream& stream, const NzMeshParams& meshParameters, const NzModelParameters& modelParameters)
{
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromStream(stream, meshParameters))
	{
		NazaraError("Failed to load mesh");
		return false;
	}

	mesh->SetPersistent(false, false);
	SetMesh(mesh.release(), modelParameters);

	return true;
}

void NzModel::Reset()
{
	m_matCount = 0;
	m_skinCount = 0;

	if (m_mesh)
	{
		m_mesh->RemoveResourceReference();
		m_mesh = nullptr;

		m_skeleton.Destroy();

		if (m_animation)
		{
			m_animation->RemoveResourceReference();
			m_animation = nullptr;
		}

		// Nous n'avons des matériaux que si nous avons un mesh
		for (const NzMaterial* material : m_materials)
			material->RemoveResourceReference();

		m_materials.clear();
	}
}

bool NzModel::SetAnimation(NzAnimation* animation)
{
	#if NAZARA_3D_SAFE
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
		m_animation->AddResourceReference();
		m_currentFrame = 0;
		m_interpolation = 0.f;

		SetSequence(0);
	}

	return true;
}

void NzModel::SetMaterial(unsigned int matIndex, NzMaterial* material)
{
	#if NAZARA_3D_SAFE
	if (matIndex >= m_matCount)
	{
		NazaraError("Material index out of range (" + NzString::Number(matIndex) + " >= " + NzString::Number(m_matCount));
		return;
	}
	#endif

	m_materials[matIndex]->RemoveResourceReference();

	if (material)
		m_materials[matIndex] = material;
	else
		m_materials[matIndex] = NzMaterial::GetDefault();

	m_materials[matIndex]->AddResourceReference();
}

void NzModel::SetMaterial(unsigned int skinIndex, unsigned int matIndex, NzMaterial* material)
{
	#if NAZARA_3D_SAFE
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

	m_materials[index]->RemoveResourceReference();

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();

	m_materials[index]->AddResourceReference();
}

void NzModel::SetMesh(NzMesh* mesh, const NzModelParameters& modelParameters)
{
	Reset();

	if (mesh)
	{
		m_mesh = mesh;
		m_mesh->AddResourceReference();

		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_skeleton = *mesh->GetSkeleton(); // Copie du squelette template

		if (modelParameters.loadAnimation && m_mesh->IsAnimable())
		{
			NzString animationPath = m_mesh->GetAnimation();
			if (!animationPath.IsEmpty())
			{
				std::unique_ptr<NzAnimation> animation(new NzAnimation);
				if (animation->LoadFromFile(animationPath, modelParameters.animationParams) && SetAnimation(animation.get()))
				{
					animation->SetPersistent(false);
					animation.release();
				}
				else
					NazaraWarning("Failed to load animation");
			}
		}

		m_matCount = mesh->GetMaterialCount();
		m_materials.resize(m_matCount, NzMaterial::GetDefault());
		if (modelParameters.loadMaterials)
		{
			for (unsigned int i = 0; i < m_matCount; ++i)
			{
				NzString mat = mesh->GetMaterial(i);
				if (!mat.IsEmpty())
				{
					std::unique_ptr<NzMaterial> material(new NzMaterial);
					if (material->LoadFromFile(mat, modelParameters.materialParams))
					{
						material->SetPersistent(false, false); // Pas de vérification des références car nous n'y avons pas encore accroché de référence
						m_materials[i] = material.release();
					}
					else
						NazaraWarning("Failed to load material #" + NzString::Number(i));
				}
			}
		}

		for (const NzMaterial* material : m_materials)
			material->AddResourceReference();
	}
}

void NzModel::SetSkinCount(unsigned int skinCount)
{
	#if NAZARA_3D_SAFE
	if (skinCount == 0)
	{
		NazaraError("Skin count must be over 0");
		return;
	}
	#endif

	m_materials.resize(m_matCount*skinCount, NzMaterial::GetDefault());
	m_skinCount = skinCount;
}

bool NzModel::SetSequence(const NzString& sequenceName)
{
	///TODO: Rendre cette erreur "safe" avec le nouveau système de gestions d'erreur (No-log)
	#if NAZARA_3D_SAFE
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
	#if NAZARA_3D_SAFE
	if (!m_animation)
	{
		NazaraError("Model has no animation");
		return;
	}
	#endif

	const NzSequence* currentSequence = m_animation->GetSequence(sequenceIndex);
	#if NAZARA_3D_SAFE
	if (!currentSequence)
	{
		NazaraError("Sequence not found");
		return;
	}
	#endif

	m_currentSequence = currentSequence;
	m_nextFrame = m_currentSequence->firstFrame;
}

void NzModel::Update(float elapsedTime)
{
	if (!m_animation)
		return;

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
}
