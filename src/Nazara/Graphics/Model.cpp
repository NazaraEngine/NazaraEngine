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
	/*!
	* \ingroup graphics
	* \class Nz::Model
	* \brief Graphics class that represents a model
	*/

	/*!
	* \brief Constructs a ModelParameters object by default
	*/

	ModelParameters::ModelParameters()
	{
		material.shaderName = "PhongLighting";
	}

	/*!
	* \brief Checks whether the parameters for the model are correct
	* \return true If parameters are valid
	*/

	bool ModelParameters::IsValid() const
	{
		if (loadMaterials && !material.IsValid())
			return false;

		return mesh.IsValid();
	}

	/*!
	* \brief Constructs a Model object by default
	*/

	Model::Model() :
	m_matCount(0),
	m_skin(0),
	m_skinCount(1)
	{
	}

	/*!
	* \brief Destructs the object and calls Reset
	*
	* \see Reset
	*/

	Model::~Model()
	{
		Reset();
	}

	/*!
	* \brief Adds this model to the render queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data used for this instance
	*/

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

			renderQueue->AddMesh(instanceData.renderOrder, material, meshData, mesh->GetAABB(), *instanceData.transformMatrix);
		}
	}

	/*!
	* \brief Gets the material of the named submesh
	* \return Pointer to the current material
	*
	* \param subMeshName Name of the subMesh
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE if there is no mesh
	* \remark Produces a NazaraError if there is no subMesh with that name
	* \remark Produces a NazaraError if material is invalid
	*/

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

		return m_materials[m_skin * m_matCount + matIndex];
	}

	/*!
	* \brief Gets the material by index
	* \return Pointer to the current material
	*
	* \param matIndex Index of the material
	*
	* \remark Produces a NazaraError if index is invalid
	*/

	Material* Model::GetMaterial(unsigned int matIndex) const
	{
		#if NAZARA_GRAPHICS_SAFE
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount) + ')');
			return nullptr;
		}
		#endif

		return m_materials[m_skin * m_matCount + matIndex];
	}

	/*!
	* \brief Gets the material by index of the named submesh
	* \return Pointer to the current material
	*
	* \param skinIndex Index of the skin
	* \param subMeshName Name of the subMesh
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if skinIndex is invalid
	* \remark Produces a NazaraError if there is no subMesh with that name
	* \remark Produces a NazaraError if material index is invalid
	*/

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

		return m_materials[skinIndex * m_matCount + matIndex];
	}

	/*!
	* \brief Gets the material by index with skin
	* \return Pointer to the current material
	*
	* \param skinIndex Index of the skin
	* \param matIndex Index of the material
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if skinIndex is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if matIndex is invalid
	*/

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

		return m_materials[skinIndex * m_matCount + matIndex];
	}

	/*!
	* \brief Gets the number of materials
	* \return Current number of materials
	*/

	unsigned int Model::GetMaterialCount() const
	{
		return m_matCount;
	}

	/*!
	* \brief Gets the mesh
	* \return Current mesh
	*/

	Mesh* Model::GetMesh() const
	{
		return m_mesh;
	}

	/*!
	* \brief Gets the skin
	* \return Current skin
	*/

	unsigned int Model::GetSkin() const
	{
		return m_skin;
	}

	/*!
	* \brief Gets the number of skins
	* \return Current number of skins
	*/

	unsigned int Model::GetSkinCount() const
	{
		return m_skinCount;
	}

	/*!
	* \brief Checks whether the model is animated
	* \return false
	*/

	bool Model::IsAnimated() const
	{
		return false;
	}

	/*!
	* \brief Loads the model from file
	* \return true if loading is successful
	*
	* \param filePath Path to the file
	* \param params Parameters for the model
	*/

	bool Model::LoadFromFile(const String& filePath, const ModelParameters& params)
	{
		return ModelLoader::LoadFromFile(this, filePath, params);
	}

	/*!
	* \brief Loads the model from memory
	* \return true if loading is successful
	*
	* \param data Raw memory
	* \param size Size of the memory
	* \param params Parameters for the model
	*/

	bool Model::LoadFromMemory(const void* data, std::size_t size, const ModelParameters& params)
	{
		return ModelLoader::LoadFromMemory(this, data, size, params);
	}

	/*!
	* \brief Loads the model from stream
	* \return true if loading is successful
	*
	* \param stream Stream to the model
	* \param params Parameters for the model
	*/

	bool Model::LoadFromStream(Stream& stream, const ModelParameters& params)
	{
		return ModelLoader::LoadFromStream(this, stream, params);
	}

	/*!
	* \brief Resets the model, cleans everything
	*/

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

	/*!
	* \brief Sets the material of the named submesh
	* \return true If successful
	*
	* \param subMeshName Name of the subMesh
	* \param material Pointer to the material
	*
	* \remark Produces a NazaraError if there is no subMesh with that name
	* \remark Produces a NazaraError if material index is invalid
	*/

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

		unsigned int index = m_skin * m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();

		return true;
	}

	/*!
	* \brief Sets the material by index
	* \return true If successful
	*
	* \param matIndex Index of the material
	* \param material Pointer to the material
	*
	* \remark Produces a NazaraError with if NAZARA_GRAPHICS_SAFE defined index is invalid
	*/

	void Model::SetMaterial(unsigned int matIndex, Material* material)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (matIndex >= m_matCount)
		{
			NazaraError("Material index out of range (" + String::Number(matIndex) + " >= " + String::Number(m_matCount));
			return;
		}
		#endif

		unsigned int index = m_skin * m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();
	}

	/*!
	* \brief Sets the material by index of the named submesh
	* \return true If successful
	*
	* \param skinIndex Index of the skin
	* \param subMeshName Name of the subMesh
	* \param material Pointer to the material
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if skinIndex is invalid
	* \remark Produces a NazaraError if there is no subMesh with that name
	* \remark Produces a NazaraError if material index is invalid
	*/

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

		unsigned int index = skinIndex * m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();

		return true;
	}

	/*!
	* \brief Sets the material by index with skin
	* \return true If successful
	*
	* \param skinIndex Index of the skin
	* \param matIndex Index of the material
	* \param material Pointer to the material
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if skinIndex is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if matIndex is invalid
	*/

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

		unsigned int index = skinIndex * m_matCount + matIndex;

		if (material)
			m_materials[index] = material;
		else
			m_materials[index] = Material::GetDefault();
	}

	/*!
	* \brief Sets the mesh
	*
	* \param pointer to the mesh
	*
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if mesh is invalid
	*/

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

	/*!
	* \brief Sets the skin
	*
	* \param skin Skin to use
	*
	* \remark Produces a NazaraError if skin is invalid
	*/

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

	/*!
	* \brief Sets the number of skins
	*
	* \param skinCount Number of skins
	*
	* \remark Produces a NazaraError if skinCount equals zero
	*/

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

	/*
	* \brief Makes the bounding volume of this billboard
	*/

	void Model::MakeBoundingVolume() const
	{
		if (m_mesh)
			m_boundingVolume.Set(m_mesh->GetAABB());
		else
			m_boundingVolume.MakeNull();
	}

	ModelLoader::LoaderList Model::s_loaders;
}
