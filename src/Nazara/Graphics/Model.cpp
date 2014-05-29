// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzModelParameters::NzModelParameters()
{
	material.shaderName = "PhongLighting";
}

bool NzModelParameters::IsValid() const
{
	if (loadMaterials && !material.IsValid())
		return false;

	return mesh.IsValid();
}

NzModel::NzModel() :
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
m_boundingVolumeUpdated(model.m_boundingVolumeUpdated),
m_matCount(model.m_matCount),
m_skin(model.m_skin),
m_skinCount(model.m_skinCount)
{
	if (model.m_mesh)
	{
		// Nous n'avons des matériaux que si nous avons un mesh
		m_mesh = model.m_mesh;
		m_materials = model.m_materials;
	}

	SetParent(model);
}

NzModel::~NzModel()
{
	Reset();
}

void NzModel::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	const NzMatrix4f& transformMatrix = GetTransformMatrix();

	unsigned int submeshCount = m_mesh->GetSubMeshCount();
	for (unsigned int i = 0; i < submeshCount; ++i)
	{
		const NzStaticMesh* mesh = static_cast<const NzStaticMesh*>(m_mesh->GetSubMesh(i));
		NzMaterial* material = m_materials[mesh->GetMaterialIndex()];

		NzMeshData meshData;
		meshData.indexBuffer = mesh->GetIndexBuffer();
		meshData.primitiveMode = mesh->GetPrimitiveMode();
		meshData.vertexBuffer = mesh->GetVertexBuffer();

		renderQueue->AddMesh(material, meshData, mesh->GetAABB(), transformMatrix);
	}
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

unsigned int NzModel::GetSkin() const
{
	return m_skin;
}

unsigned int NzModel::GetSkinCount() const
{
	return m_skinCount;
}

bool NzModel::IsAnimated() const
{
	return false;
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
	m_matCount = 0;
	m_skinCount = 0;

	if (m_mesh)
	{
		m_mesh.Reset();
		m_materials.clear();
	}
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
	#if NAZARA_GRAPHICS_SAFE
	if (mesh && !mesh->IsValid())
	{
		NazaraError("Invalid mesh");
		return;
	}
	#endif

	m_mesh = mesh;

	if (m_mesh)
	{
		m_matCount = mesh->GetMaterialCount();
		m_materials.clear();
		m_materials.resize(m_matCount, NzMaterial::GetDefault());
		m_skinCount = 1;
	}
	else
	{
		m_matCount = 0;
		m_materials.clear();
		m_skinCount = 0;
	}

	InvalidateBoundingVolume();
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

	m_boundingVolume = node.m_boundingVolume;
	m_boundingVolumeUpdated = node.m_boundingVolumeUpdated;
	m_matCount = node.m_matCount;
	m_materials = node.m_materials;
	m_mesh = node.m_mesh;
	m_skin = node.m_skin;
	m_skinCount = node.m_skinCount;

	return *this;
}

NzModel& NzModel::operator=(NzModel&& node)
{
	NzSceneNode::operator=(node);

	// Ressources
	m_mesh = std::move(node.m_mesh);
	m_materials = std::move(node.m_materials);

	// Paramètres
	m_boundingVolume = node.m_boundingVolume;
	m_boundingVolumeUpdated = node.m_boundingVolumeUpdated;
	m_matCount = node.m_matCount;
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

void NzModel::InvalidateNode()
{
	NzSceneNode::InvalidateNode();

	m_boundingVolumeUpdated = false;
}

void NzModel::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
		m_boundingVolume.Set(m_mesh->GetAABB());

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingVolume.Update(m_transformMatrix);
	m_boundingVolumeUpdated = true;
}

NzModelLoader::LoaderList NzModel::s_loaders;
