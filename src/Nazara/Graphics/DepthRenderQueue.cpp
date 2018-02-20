// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DepthRenderQueue
	* \brief Graphics class that represents the rendering queue for depth rendering
	*/

	/*!
	* \brief Constructs a DepthRenderTechnique object by default
	*/

	DepthRenderQueue::DepthRenderQueue()
	{
		// Material
		m_baseMaterial = Material::New();
		m_baseMaterial->EnableColorWrite(false);
		m_baseMaterial->EnableFaceCulling(false);
		//m_baseMaterial->SetFaceCulling(FaceSide_Front);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Sizes of the billboards
	* \param sinCosPtr Rotation of the billboards if null, Vector2f(0.f, 1.f) is used
	* \param colorPtr Color of the billboards if null, Color::White is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Sizes of the billboards
	* \param sinCosPtr Rotation of the billboards if null, Vector2f(0.f, 1.f) is used
	* \param alphaPtr Alpha parameters of the billboards if null, 1.f is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Sizes of the billboards
	* \param anglePtr Rotation of the billboards if null, 0.f is used
	* \param colorPtr Color of the billboards if null, Color::White is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Sizes of the billboards
	* \param anglePtr Rotation of the billboards if null, 0.f is used
	* \param alphaPtr Alpha parameters of the billboards if null, 1.f is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Size of the billboards
	* \param sinCosPtr Rotation of the billboards if null, Vector2f(0.f, 1.f) is used
	* \param colorPtr Color of the billboards if null, Color::White is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Size of the billboards
	* \param sinCosPtr Rotation of the billboards if null, Vector2f(0.f, 1.f) is used
	* \param alphaPtr Alpha parameters of the billboards if null, 1.f is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Size of the billboards
	* \param anglePtr Rotation of the billboards if null, 0.f is used
	* \param colorPtr Color of the billboards if null, Color::White is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
	}

	/*!
	* \brief Adds multiple billboards to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboards
	* \param count Number of billboards
	* \param positionPtr Position of the billboards
	* \param sizePtr Size of the billboards
	* \param anglePtr Rotation of the billboards if null, 0.f is used
	* \param alphaPtr Alpha parameters of the billboards if null, 1.f is used
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddBillboards(0, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	/*!
	* \brief Adds a direcitonal light to the queue
	*
	* \param light Light to add
	*
	* \remark Produces a NazaraAssert
	*/

	void DepthRenderQueue::AddDirectionalLight(const DirectionalLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	/*!
	* \brief Adds mesh to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the mesh
	* \param meshData Data of the mesh
	* \param meshAABB Box of the mesh
	* \param transformMatrix Matrix of the mesh
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);
		NazaraUnused(meshAABB);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddMesh(0, material, meshData, meshAABB, transformMatrix, scissorRect);
	}

	/*!
	* \brief Adds a point light to the queue
	*
	* \param light Light to add
	*
	* \remark Produces a NazaraAssert
	*/

	void DepthRenderQueue::AddPointLight(const PointLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	/*!
	* \brief Adds a spot light to the queue
	*
	* \param light Light to add
	*
	* \remark Produces a NazaraAssert
	*/

	void DepthRenderQueue::AddSpotLight(const SpotLight& light)
	{
		NazaraAssert(false, "Depth render queue doesn't handle lights");
		NazaraUnused(light);
	}

	/*!
	* \brief Adds sprites to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the sprites
	* \param vertices Buffer of data for the sprites
	* \param spriteCount Number of sprites
	* \param overlay Texture of the sprites
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void DepthRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay /*= nullptr*/)
	{
		NazaraAssert(material, "Invalid material");
		NazaraUnused(renderOrder);
		NazaraUnused(overlay);

		if (!IsMaterialSuitable(material))
			return;

		if (material->HasDepthMaterial())
			material = material->GetDepthMaterial();
		else
			material = m_baseMaterial;

		ForwardRenderQueue::AddSprites(0, material, vertices, spriteCount, scissorRect, overlay);
	}
}

