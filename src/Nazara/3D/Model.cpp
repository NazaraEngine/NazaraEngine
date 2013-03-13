// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Model.hpp>
#include <Nazara/3D/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <tuple>
#include <Nazara/3D/Debug.hpp>

NzModel::NzModel() :
m_animation(nullptr),
m_mesh(nullptr),
m_currentSequence(nullptr),
m_animationEnabled(true),
m_boundingBoxUpdated(false),
m_drawEnabled(true),
m_matCount(0),
m_skin(0),
m_skinCount(1)
{
}

NzModel::NzModel(const NzModel& model) :
NzSceneNode(model),
m_materials(model.m_materials),
m_boundingBox(model.m_boundingBox),
m_animation(model.m_animation),
m_mesh(model.m_mesh),
m_currentSequence(model.m_currentSequence),
m_animationEnabled(model.m_animationEnabled),
m_boundingBoxUpdated(model.m_boundingBoxUpdated),
m_drawEnabled(model.m_drawEnabled),
m_interpolation(model.m_interpolation),
m_currentFrame(model.m_currentFrame),
m_matCount(model.m_matCount),
m_nextFrame(model.m_nextFrame),
m_skin(model.m_skin),
m_skinCount(model.m_skinCount)
{
	if (m_mesh)
	{
		if (m_animation)
			m_animation->AddResourceReference();

		m_mesh->AddResourceReference();

		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_skeleton = model.m_skeleton;

		// Nous n'avons des matériaux que si nous avons un mesh
		for (const NzMaterial* material : m_materials)
			material->AddResourceReference();
	}
}

NzModel::~NzModel()
{
	Reset();
}

void NzModel::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	unsigned int subMeshCount = m_mesh->GetSubMeshCount();
	for (unsigned int i = 0; i < subMeshCount; ++i)
	{
		NzSubMesh* subMesh = m_mesh->GetSubMesh(i);
		NzMaterial* material = m_materials[m_skin*m_matCount + subMesh->GetMaterialIndex()];

		switch (subMesh->GetAnimationType())
		{
			case nzAnimationType_Skeletal:
			{
				NzSkeletalMesh* skeletalMesh = static_cast<NzSkeletalMesh*>(subMesh);
				std::vector<NzRenderQueue::SkeletalData>& data = renderQueue.visibleSkeletalModels[material][skeletalMesh];

				///TODO: Corriger cette abomination
				data.resize(data.size()+1);
				NzRenderQueue::SkeletalData& skeletalData = data.back();

				skeletalData.skinnedVertices.resize(skeletalMesh->GetVertexCount());
				skeletalData.transformMatrix = m_transformMatrix;

				skeletalMesh->Skin(&skeletalData.skinnedVertices[0], &m_skeleton);
				break;
			}

			case nzAnimationType_Static:
			{
				NzStaticMesh* staticMesh = static_cast<NzStaticMesh*>(subMesh);
				std::vector<NzMatrix4f>& matrices = renderQueue.visibleStaticModels[material][staticMesh];

				matrices.push_back(m_transformMatrix);
				break;
			}
		}
	}
}

void NzModel::AdvanceAnimation(float elapsedTime)
{
	#if NAZARA_3D_SAFE
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
	m_boundingBox.MakeNull();
	m_boundingBoxUpdated = false;
}

void NzModel::EnableAnimation(bool animation)
{
	m_animationEnabled = animation;
}

void NzModel::EnableDraw(bool draw)
{
	m_drawEnabled = draw;
}

NzAnimation* NzModel::GetAnimation() const
{
	return m_animation;
}

const NzBoundingBoxf& NzModel::GetBoundingBox() const
{
	#if NAZARA_3D_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no mesh");

		static NzBoundingBoxf dummy(nzExtend_Null);
		return dummy;
	}
	#endif

	if (!m_boundingBoxUpdated)
		UpdateBoundingBox();

	return m_boundingBox;
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

	return m_materials[m_skin*m_matCount + matIndex];
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

bool NzModel::IsDrawEnabled() const
{
	return m_drawEnabled;
}

bool NzModel::LoadFromFile(const NzString& meshPath, const NzModelParameters& modelParameters)
{
	///TODO: ResourceManager
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromFile(meshPath, modelParameters.mesh))
	{
		NazaraError("Failed to load mesh");
		return false;
	}

	mesh->SetPersistent(false, false);
	SetMesh(mesh.release(), modelParameters);

	return true;
}

bool NzModel::LoadFromMemory(const void* data, std::size_t size, const NzModelParameters& modelParameters)
{
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromMemory(data, size, modelParameters.mesh))
	{
		NazaraError("Failed to load mesh");
		return false;
	}

	mesh->SetPersistent(false, false);
	SetMesh(mesh.release(), modelParameters);

	return true;
}

bool NzModel::LoadFromStream(NzInputStream& stream, const NzModelParameters& modelParameters)
{
	std::unique_ptr<NzMesh> mesh(new NzMesh);
	if (!mesh->LoadFromStream(stream, modelParameters.mesh))
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
	if (m_scene)
		m_scene->UnregisterForUpdate(this);

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

		if (m_scene)
			m_scene->RegisterForUpdate(this);
	}
	else if (m_scene)
		m_scene->UnregisterForUpdate(this);

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

	unsigned int index = m_skin*m_matCount + matIndex;

	m_materials[index]->RemoveResourceReference();

	if (material)
		m_materials[index] = material;
	else
		m_materials[index] = NzMaterial::GetDefault();

	m_materials[index]->AddResourceReference();
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
		m_boundingBoxUpdated = false;
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
				if (animation->LoadFromFile(animationPath, modelParameters.animation) && SetAnimation(animation.get()))
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
					if (material->LoadFromFile(mat, modelParameters.material))
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

void NzModel::SetSkin(unsigned int skin)
{
	#if NAZARA_3D_SAFE
	if (skin >= m_skinCount)
	{
		NazaraError("Skin index out of range (" + NzString::Number(skin) + " >= " + NzString::Number(m_skinCount));
		return;
	}
	#endif

	m_skin = skin;
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

void NzModel::Invalidate()
{
	NzSceneNode::Invalidate();

	m_boundingBoxUpdated = false;
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

void NzModel::UpdateBoundingBox() const
{
	if (m_boundingBox.IsNull())
	{
		if (m_mesh->GetAnimationType() == nzAnimationType_Skeletal)
			m_boundingBox.Set(m_skeleton.GetAABB());
		else
			m_boundingBox.Set(m_mesh->GetAABB());
	}

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingBox.Update(m_transformMatrix);
	m_boundingBoxUpdated = true;
}

bool NzModel::VisibilityTest(const NzFrustumf& frustum)
{
	#if NAZARA_3D_SAFE
	if (!m_mesh)
	{
		NazaraError("Model has no mesh");
		return false;
	}
	#endif

	if (!m_drawEnabled)
		return false;

	if (!m_boundingBoxUpdated)
		UpdateBoundingBox();

	return frustum.Contains(m_boundingBox);
}
