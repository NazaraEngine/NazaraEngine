// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredProxyRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/BasicRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DeferredProxyRenderQueue
	* \brief Graphics class sorting the objects into a deferred and forward render queue (depending on blending)
	*/

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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, colorPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, colorPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, alphaPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, alphaPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, colorPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, colorPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, alphaPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, alphaPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, colorPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, colorPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, alphaPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, sinCosPtr, alphaPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, colorPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, colorPtr);
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
	*/

	void DeferredProxyRenderQueue::AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, alphaPtr);
		else
			m_forwardRenderQueue->AddBillboards(renderOrder, material, billboardCount, scissorRect, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	/*!
	* \brief Adds drawable to the queue
	*
	* \param renderOrder Order of rendering
	* \param drawable Drawable user defined
	*
	* \remark Produces a NazaraError if drawable is invalid
	*/

	void DeferredProxyRenderQueue::AddDrawable(int renderOrder, const Drawable* drawable)
	{
		m_forwardRenderQueue->AddDrawable(renderOrder, drawable);
	}

	/*!
	* \brief Adds mesh to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the mesh
	* \param meshData Data of the mesh
	* \param meshAABB Box of the mesh
	* \param transformMatrix Matrix of the mesh
	*/

	void DeferredProxyRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddMesh(renderOrder, material, meshData, meshAABB, transformMatrix, scissorRect);
		else
			m_forwardRenderQueue->AddMesh(renderOrder, material, meshData, meshAABB, transformMatrix, scissorRect);
	}

	/*!
	* \brief Adds sprites to the queue
	*
	* \param renderOrder Order of rendering
	* \param material Material of the sprites
	* \param vertices Buffer of data for the sprites
	* \param spriteCount Number of sprites
	* \param overlay Texture of the sprites
	*/

	void DeferredProxyRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay)
	{
		NazaraAssert(material, "Invalid material");

		if (!material->IsBlendingEnabled())
			m_deferredRenderQueue->AddSprites(renderOrder, material, vertices, spriteCount, scissorRect, overlay);
		else
			m_forwardRenderQueue->AddSprites(renderOrder, material, vertices, spriteCount, scissorRect, overlay);
	}

	/*!
	* \brief Clears the queue
	*
	* \param fully Should everything be cleared or we can keep layers
	*/

	void DeferredProxyRenderQueue::Clear(bool fully)
	{
		AbstractRenderQueue::Clear(fully);

		m_deferredRenderQueue->Clear(fully);
		m_forwardRenderQueue->Clear(fully);
	}
}
