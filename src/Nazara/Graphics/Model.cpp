// Copyright (C) 2017 Jérôme Leclercq
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
	* \brief Destructs the object and cleans resources
	*/
	Model::~Model() = default;

	/*!
	* \brief Adds this model to the render queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data used for this instance
	*/
	void Model::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const
	{
		unsigned int submeshCount = m_mesh->GetSubMeshCount();
		for (unsigned int i = 0; i < submeshCount; ++i)
		{
			const StaticMesh* mesh = static_cast<const StaticMesh*>(m_mesh->GetSubMesh(i));
			const MaterialRef& material = GetMaterial(mesh->GetMaterialIndex());

			MeshData meshData;
			meshData.indexBuffer = mesh->GetIndexBuffer();
			meshData.primitiveMode = mesh->GetPrimitiveMode();
			meshData.vertexBuffer = mesh->GetVertexBuffer();

			renderQueue->AddMesh(instanceData.renderOrder, material, meshData, mesh->GetAABB(), instanceData.transformMatrix, scissorRect);
		}
	}

	/*!
	* \brief Clones this model
	*/
	std::unique_ptr<InstancedRenderable> Model::Clone() const
	{
		return std::make_unique<Model>(*this);
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
	const MaterialRef& Model::GetMaterial(const String& subMeshName) const
	{
		NazaraAssert(m_mesh, "Model has no mesh");

		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');

			static MaterialRef Invalid;
			return Invalid;
		}

		return GetMaterial(subMesh->GetMaterialIndex());
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
	const MaterialRef& Model::GetMaterial(std::size_t skinIndex, const String& subMeshName) const
	{
		NazaraAssert(m_mesh, "Model has no mesh");

		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');

			static MaterialRef Invalid;
			return Invalid;
		}

		return GetMaterial(subMesh->GetMaterialIndex());
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
	* \brief Sets the material of the named submesh
	* \return true If successful
	*
	* \param subMeshName Name of the subMesh
	* \param material Pointer to the material
	*
	* \remark Produces a NazaraError if there is no subMesh with that name
	* \remark Produces a NazaraError if material index is invalid
	*/

	bool Model::SetMaterial(const String& subMeshName, MaterialRef material)
	{
		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return false;
		}

		SetMaterial(subMesh->GetMaterialIndex(), std::move(material));
		return true;
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
	bool Model::SetMaterial(std::size_t skinIndex, const String& subMeshName, MaterialRef material)
	{
		SubMesh* subMesh = m_mesh->GetSubMesh(subMeshName);
		if (!subMesh)
		{
			NazaraError("Mesh has no submesh \"" + subMeshName + '"');
			return false;
		}

		SetMaterial(skinIndex, subMesh->GetMaterialIndex(), std::move(material));
		return true;
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
			ResetMaterials(mesh->GetMaterialCount());
			m_meshAABBInvalidationSlot.Connect(m_mesh->OnMeshInvalidateAABB, [this](const Nz::Mesh*) { InvalidateBoundingVolume(); });
		}
		else
		{
			ResetMaterials(0);
			m_meshAABBInvalidationSlot.Disconnect();
		}

		InvalidateBoundingVolume();
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

	ModelLibrary::LibraryMap Model::s_library;
	ModelLoader::LoaderList Model::s_loaders;
	ModelManager::ManagerMap Model::s_managerMap;
	ModelManager::ManagerParams Model::s_managerParameters;
	ModelSaver::SaverList Model::s_savers;
}
