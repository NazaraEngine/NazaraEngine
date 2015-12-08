// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ModelParameters::ModelParameters()
	{
		material.shaderName = "PhongLighting";
	}

	bool ModelParameters::IsValid() const
	{
		if (loadMaterials && !material.IsValid())
			return false;

		return mesh.IsValid();
	}

	Model::Model() :
	m_matCount(0),
	m_skin(0),
	m_skinCount(1)
	{
	}

	Model::~Model()
	{
		Reset();
	}

	void Model::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		unsigned int submeshCount = m_mesh->GetSubMeshCount();
		for (unsigned int i = 0; i < submeshCount; ++i)
		{
			const StaticMesh* mesh = static_cast<const StaticMesh*>(m_mesh->GetSubMesh(i));
			Material* material = m_materials[mesh->GetMaterialIndex()];

			MeshData meshData;
			meshData.indexBuffer = mesh->GetIndexBuffer();
			meshData.primitiveMode = mesh->GetPrimitiveMode();
			meshData.vertexBuffer = mesh->GetVertexBuffer();

			renderQueue->AddMesh(instanceData.renderOrder, material, meshData, mesh->GetAABB(), instanceData.transformMatrix);
		}
	}

	Material* Model::GetMaterial(const String& subMeshName) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!m_mesh)
		{
			NazaraError("Model has no mesh");
			return nullptr;
		}
		#endif

		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return nullptr;
		}

		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return nullptr;
		}

		return m_materials[m_skin*m_matCount + matIndex];
	}

	Material* Model::GetMaterial(unsigned int matIndex) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return nullptr;
		}
		#endif

		return m_materials[m_skin*m_matCount + matIndex];
	}

	Material* Model::GetMaterial(unsigned int skinIndex, const String& subMeshName) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skinIndex >= m_skinCount)
		{
			NazaraError("Skin index out of range (" + String::Number(skinIndex) + " >= " + String::Number(m_skinCount) + ')');
			return nullptr;
		}
		#endif

		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return nullptr;
		}

		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return nullptr;
		}

		return m_materials[skinIndex*m_matCount + matIndex];
	}

	Material* Model::GetMaterial(unsigned int skinIndex, unsigned int matIndex) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skinIndex >= m_skinCount)
		{
			NazaraError("Skin index out of range (" + String::Number(skinIndex) + " >= " + String::Number(m_skinCount) + ')');
			return nullptr;
		}

		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return nullptr;
		}
		#endif

		return m_materials[skinIndex*m_matCount + matIndex];
	}

	unsigned int Model::GetMaterialCount() const
	{
		return m_matCount;
	}

	Mesh* Model::GetMesh() const
	{
		return m_mesh;
	}

	unsigned int Model::GetSkin() const
	{
		return m_skin;
	}

	unsigned int Model::GetSkinCount() const
	{
		return m_skinCount;
	}

	bool Model::IsAnimated() const
	{
		return false;
	}

	bool Model::LoadFromFile(const String& filePath, const ModelParameters& params)
	{
		return ModelLoader::LoadFromFile(this, filePath, params);
	}

	bool Model::LoadFromMemory(const void* data, std::size_t size, const ModelParameters& params)
	{
		return ModelLoader::LoadFromMemory(this, data, size, params);
	}

	bool Model::LoadFromStream(Stream& stream, const ModelParameters& params)
	{
		return ModelLoader::LoadFromStream(this, stream, params);
	}

	void Model::Reset()
	{
		m_matCount = 0;
		m_skinCount = 0;

		if (m_mesh)
		{
			m_mesh.Reset();
			m_materials.clear();
		}
	}

	bool Model::SetMaterial(const String& subMeshName, Material* material)
	{
		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return false;
		}

		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return false;
		}

		unsigned int index = m_skin*m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();

		return true;
	}

	void Model::SetMaterial(unsigned int matIndex, Material* material)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount));
			return;
		}
		#endif

		unsigned int index = m_skin*m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();
	}

	bool Model::SetMaterial(unsigned int skinIndex, const String& subMeshName, Material* material)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skinIndex >= m_skinCount)
		{
			NazaraError("Skin index out of range (" + String::Number(skinIndex) + " >= " + String::Number(m_skinCount));
			return false;
		}
		#endif

		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return false;
		}

		unsigned int matIndex = subMesh->GetMaterialIndex();
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount));
			return false;
		}

		unsigned int index = skinIndex*m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();

		return true;
	}

	void Model::SetMaterial(unsigned int skinIndex, unsigned int matIndex, Material* material)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skinIndex >= m_skinCount)
		{
			NazaraError("Skin index out of range (" + String::Number(skinIndex) + " >= " + String::Number(m_skinCount));
			return;
		}

		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount));
			return;
		}
		#endif

		unsigned int index = skinIndex*m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();
	}

	void Model::SetMesh(Mesh* mesh)
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
			m_materials.resize(m_matCount, Material::GetDefault());
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

	void Model::SetSkin(unsigned int skin)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skin >= m_skinCount)
		{
			NazaraError("Skin index out of range (" + String::Number(skin) + " >= " + String::Number(m_skinCount) + ')');
			return;
		}
		#endif

		m_skin = skin;
	}

	void Model::SetSkinCount(unsigned int skinCount)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (skinCount == 0)
		{
			NazaraError("Skin count must be over zero");
			return;
		}
		#endif

		m_materials.resize(m_matCount*skinCount, Material::GetDefault());
		m_skinCount = skinCount;
	}

	void Model::MakeBoundingVolume() const
	{
		if (m_mesh)
			m_boundingVolume.Set(m_mesh->GetAABB());
		else
			m_boundingVolume.MakeNull();
	}

	ModelLoader::LoaderList Model::s_loaders;
}
