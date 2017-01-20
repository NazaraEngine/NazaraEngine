// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Render billboards using Deferred Shading if possible

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::DeferredRenderQueue
	* \brief Graphics class that represents the rendering queue for deferred rendering
	*/

	/*!
	* \brief Constructs a DeferredRenderQueue object with the rendering queue of forward rendering
	*
	* \param forwardQueue Queue of data to render
	*/

	DeferredRenderQueue::DeferredRenderQueue(ForwardRenderQueue* forwardQueue) :
	m_forwardQueue(forwardQueue)
	{
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
	*/

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, colorPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, sinCosPtr, alphaPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, colorPtr);
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

	void DeferredRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		m_forwardQueue->AddBillboards(renderOrder, material, count, positionPtr, sizePtr, anglePtr, alphaPtr);
	}

	/*!
	* \brief Adds drawable to the queue
	*
	* \param renderOrder Order of rendering
	* \param drawable Drawable user defined
	*
	* \remark Produces a NazaraError if drawable is invalid
	*/

	void DeferredRenderQueue::AddDrawable(int renderOrder, const Drawable* drawable)
	{
		m_forwardQueue->AddDrawable(renderOrder, drawable);
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

	void DeferredRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix)
	{
		if (material->IsBlendingEnabled())
			// One transparent material ? I don't like it, go see if I'm in the forward queue
			m_forwardQueue->AddMesh(renderOrder, material, meshData, meshAABB, transformMatrix);
		else
		{
			Layer& currentLayer = GetLayer(renderOrder);
			MeshPipelineBatches& opaqueModels = currentLayer.opaqueModels;

			const MaterialPipeline* materialPipeline = material->GetPipeline();

			auto pipelineIt = opaqueModels.find(materialPipeline);
			if (pipelineIt == opaqueModels.end())
			{
				BatchedMaterialEntry materialEntry;
				pipelineIt = opaqueModels.insert(MeshPipelineBatches::value_type(materialPipeline, std::move(materialEntry))).first;
			}

			BatchedMaterialEntry& materialEntry = pipelineIt->second;
			MeshMaterialBatches& materialMap = materialEntry.materialMap;

			auto materialIt = materialMap.find(material);
			if (materialIt == materialMap.end())
			{
				BatchedModelEntry entry;
				entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &DeferredRenderQueue::OnMaterialInvalidation);

				materialIt = materialMap.insert(MeshMaterialBatches::value_type(material, std::move(entry))).first;
			}

			BatchedModelEntry& entry = materialIt->second;
			entry.enabled = true;

			MeshInstanceContainer& meshMap = entry.meshMap;

			auto it2 = meshMap.find(meshData);
			if (it2 == meshMap.end())
			{
				MeshInstanceEntry instanceEntry;
				if (meshData.indexBuffer)
					instanceEntry.indexBufferReleaseSlot.Connect(meshData.indexBuffer->OnIndexBufferRelease, this, &DeferredRenderQueue::OnIndexBufferInvalidation);

				instanceEntry.vertexBufferReleaseSlot.Connect(meshData.vertexBuffer->OnVertexBufferRelease, this, &DeferredRenderQueue::OnVertexBufferInvalidation);

				it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
			}

			std::vector<Matrix4f>& instances = it2->second.instances;
			instances.push_back(transformMatrix);

			materialEntry.maxInstanceCount = std::max(materialEntry.maxInstanceCount, instances.size());
		}
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

	void DeferredRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const Texture* overlay)
	{
		m_forwardQueue->AddSprites(renderOrder, material, vertices, spriteCount, overlay);
	}

	/*!
	* \brief Clears the queue
	*
	* \param fully Should everything be cleared or we can keep layers
	*/

	void DeferredRenderQueue::Clear(bool fully)
	{
		AbstractRenderQueue::Clear(fully);

		if (fully)
			layers.clear();
		else
		{
			for (auto it = layers.begin(); it != layers.end();)
			{
				Layer& layer = it->second;
				if (layer.clearCount++ >= 100)
					it = layers.erase(it);
				else
				{
					for (auto& pipelinePair : layer.opaqueModels)
					{
						auto& pipelineEntry = pipelinePair.second;

						if (pipelineEntry.maxInstanceCount > 0)
						{
							for (auto& materialPair : pipelineEntry.materialMap)
							{
								auto& matEntry = materialPair.second;

								if (matEntry.enabled)
								{
									MeshInstanceContainer& meshInstances = matEntry.meshMap;

									for (auto& meshIt : meshInstances)
									{
										auto& meshEntry = meshIt.second;

										meshEntry.instances.clear();
									}
									matEntry.enabled = false;
								}
							}
							pipelineEntry.maxInstanceCount = 0;
						}
					}

					++it;
				}
			}
		}

		m_forwardQueue->Clear(fully);
	}

	/*!
	* \brief Gets the ith layer
	* \return Reference to the ith layer for the queue
	*
	* \param i Index of the layer
	*/

	DeferredRenderQueue::Layer& DeferredRenderQueue::GetLayer(unsigned int i)
	{
		auto it = layers.find(i);
		if (it == layers.end())
			it = layers.insert(std::make_pair(i, Layer())).first;

		Layer& layer = it->second;
		layer.clearCount = 0;

		return layer;
	}
    
	/*!
	* \brief Handle the invalidation of an index buffer
	*
	* \param indexBuffer Index buffer being invalidated
	*/

	void DeferredRenderQueue::OnIndexBufferInvalidation(const IndexBuffer* indexBuffer)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			for (auto& pipelineEntry : layer.opaqueModels)
			{
				for (auto& materialEntry : pipelineEntry.second.materialMap)
				{
					MeshInstanceContainer& meshes = materialEntry.second.meshMap;
					for (auto it = meshes.begin(); it != meshes.end();)
					{
						const MeshData& renderData = it->first;
						if (renderData.indexBuffer == indexBuffer)
							it = meshes.erase(it);
						else
							++it;
					}
				}
			}
		}
	}

	/*!
	* \brief Handle the invalidation of a material
	*
	* \param material Material being invalidated
	*/

	void DeferredRenderQueue::OnMaterialInvalidation(const Material* material)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			for (auto& pipelineEntry : layer.opaqueModels)
				pipelineEntry.second.materialMap.erase(material);
		}
	}

	/*!
	* \brief Handle the invalidation of a vertex buffer
	*
	* \param vertexBuffer Vertex buffer being invalidated
	*/

	void DeferredRenderQueue::OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;
			for (auto& pipelineEntry : layer.opaqueModels)
			{
				for (auto& materialEntry : pipelineEntry.second.materialMap)
				{
					MeshInstanceContainer& meshes = materialEntry.second.meshMap;
					for (auto it = meshes.begin(); it != meshes.end();)
					{
						const MeshData& renderData = it->first;
						if (renderData.vertexBuffer == vertexBuffer)
							it = meshes.erase(it);
						else
							++it;
					}
				}
			}
		}
	}
}
