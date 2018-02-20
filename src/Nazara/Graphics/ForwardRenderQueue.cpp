// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Replace sinus/cosinus by a lookup table (which will lead to a speed up about 10x)

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ForwardRenderQueue
	* \brief Graphics class that represents the rendering queue for forward rendering
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
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size = *sizePtr++;
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size = *sizePtr++;
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos.Set(sin, cos);
			billboardData->size = *sizePtr++;
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos.Set(sin, cos);
			billboardData->size = *sizePtr++;
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		Vector2f defaultSinCos(0.f, 1.f); // sin(0) = 0, cos(0) = 1

		if (!sinCosPtr)
			sinCosPtr.Reset(&defaultSinCos, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos = *sinCosPtr++;
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr)
	{
		NazaraAssert(material, "Invalid material");

		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		if (!colorPtr)
			colorPtr.Reset(&Color::White, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = *colorPtr++;
			billboardData->sinCos.Set(sin, cos);
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
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

	void ForwardRenderQueue::AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr)
	{
		NazaraAssert(material, "Invalid material");

		float defaultRotation = 0.f;

		if (!anglePtr)
			anglePtr.Reset(&defaultRotation, 0); // The trick here is to put the stride to zero, which leads the pointer to be immobile

		float defaultAlpha = 1.f;

		if (!alphaPtr)
			alphaPtr.Reset(&defaultAlpha, 0); // Same

		BillboardData* billboardData = GetBillboardData(renderOrder, material, count);
		for (unsigned int i = 0; i < count; ++i)
		{
			float sin = std::sin(ToRadians(*anglePtr));
			float cos = std::cos(ToRadians(*anglePtr));
			anglePtr++;

			billboardData->center = *positionPtr++;
			billboardData->color = Color(255, 255, 255, static_cast<UInt8>(255.f * (*alphaPtr++)));
			billboardData->sinCos.Set(sin, cos);
			billboardData->size.Set(*sizePtr++);
			billboardData++;
		}
	}

	/*!
	* \brief Adds drawable to the queue
	*
	* \param renderOrder Order of rendering
	* \param drawable Drawable user defined
	*
	* \remark Produces a NazaraError if drawable is invalid
	*/

	void ForwardRenderQueue::AddDrawable(int renderOrder, const Drawable* drawable)
	{
		#if NAZARA_GRAPHICS_SAFE
		if (!drawable)
		{
			NazaraError("Invalid drawable");
			return;
		}
		#endif

		auto& otherDrawables = GetLayer(renderOrder).otherDrawables;

		otherDrawables.push_back(drawable);
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
	void ForwardRenderQueue::AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect)
	{
		NazaraAssert(material, "Invalid material");

		/*auto it = m_materialIds.find(material);
		if (it != m_materialIds.end())
			it = m_materialIds.emplace(material, m_materialIds.size()).first;

		std::size_t matId = it->second;
*/
		RegisterLayer(renderOrder);

		if (material->IsDepthSortingEnabled())
		{
			Layer& currentLayer = GetLayer(renderOrder);
			auto& transparentMeshes = currentLayer.depthSortedMeshes;
			auto& transparentData = currentLayer.depthSortedMeshData;

			// The material is marked for depth sorting, we must draw this mesh using another way (after the rendering of opaques objects while sorting them)
			std::size_t index = transparentData.size();
			transparentData.resize(index+1);

			UnbatchedModelData& data = transparentData.back();
			data.material = material;
			data.meshData = meshData;
			data.obbSphere = Spheref(transformMatrix.GetTranslation() + meshAABB.GetCenter(), meshAABB.GetSquaredRadius());
			data.transformMatrix = transformMatrix;

			transparentMeshes.push_back(index);
		}
		else
		{
			opaqueModels.Insert({
				renderOrder,
				meshData,
				material,
				transformMatrix,
				scissorRect
			});

			/*Layer& currentLayer = GetLayer(renderOrder);
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
				entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

				materialIt = materialMap.insert(MeshMaterialBatches::value_type(material, std::move(entry))).first;
			}

			BatchedModelEntry& entry = materialIt->second;
			entry.enabled = true;

			MeshInstanceContainer& meshMap = entry.meshMap;

			auto it2 = meshMap.find(meshData);
			if (it2 == meshMap.end())
			{
				MeshInstanceEntry instanceEntry;
				instanceEntry.squaredBoundingSphere = meshAABB.GetSquaredBoundingSphere();

				if (meshData.indexBuffer)
					instanceEntry.indexBufferReleaseSlot.Connect(meshData.indexBuffer->OnIndexBufferRelease, this, &ForwardRenderQueue::OnIndexBufferInvalidation);

				instanceEntry.vertexBufferReleaseSlot.Connect(meshData.vertexBuffer->OnVertexBufferRelease, this, &ForwardRenderQueue::OnVertexBufferInvalidation);

				it2 = meshMap.insert(std::make_pair(meshData, std::move(instanceEntry))).first;
			}

			std::vector<Matrix4f>& instances = it2->second.instances;
			instances.push_back(transformMatrix);

			materialEntry.maxInstanceCount = std::max(materialEntry.maxInstanceCount, instances.size());*/
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
	*
	* \remark Produces a NazaraAssert if material is invalid
	*/
	void ForwardRenderQueue::AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay /*= nullptr*/)
	{
		NazaraAssert(material, "Invalid material");

		Layer& currentLayer = GetLayer(renderOrder);

		if (material->IsDepthSortingEnabled())
		{
			auto& transparentSprites = currentLayer.depthSortedSprites;
			auto& transparentData = currentLayer.depthSortedSpriteData;

			// The material is marked for depth sorting, we must draw this mesh using another way (after the rendering of opaques objects while sorting them)
			std::size_t index = transparentData.size();
			transparentData.resize(index + 1);

			UnbatchedSpriteData& data = transparentData.back();
			data.material = material;
			data.overlay = overlay;
			data.spriteCount = spriteCount;
			data.vertices = vertices;

			transparentSprites.push_back(index);
		}
		else
		{
			basicSprites.Insert({
				renderOrder,
				spriteCount,
				material,
				overlay,
				vertices,
				scissorRect
			});
		}
	}

	/*!
	* \brief Clears the queue
	*
	* \param fully Should everything be cleared or we can keep layers
	*/

	void ForwardRenderQueue::Clear(bool fully)
	{
		AbstractRenderQueue::Clear(fully);

		basicSprites.Clear();
		opaqueModels.Clear();

		m_renderLayers.clear();
		if (fully)
			layers.clear();
		else
		{
			for (auto it = layers.begin(); it != layers.end();)
			{
				Layer& layer = it->second;
				if (layer.clearCount++ >= 100)
					layers.erase(it++);
				else
				{
					for (auto& pipelinePair : layer.billboards)
					{
						auto& pipelineEntry = pipelinePair.second;

						if (pipelineEntry.enabled)
						{
							for (auto& matIt : pipelinePair.second.materialMap)
							{
								auto& entry = matIt.second;
								auto& billboardVector = entry.billboards;

								billboardVector.clear();
							}
						}

						pipelineEntry.enabled = false;
					}

					for (auto& pipelinePair : layer.opaqueSprites)
					{
						auto& pipelineEntry = pipelinePair.second;

						if (pipelineEntry.enabled)
						{
							for (auto& materialPair : pipelineEntry.materialMap)
							{
								auto& matEntry = materialPair.second;

								if (matEntry.enabled)
								{
									auto& overlayMap = matEntry.overlayMap;
									for (auto& overlayIt : overlayMap)
									{
										auto& spriteChainVector = overlayIt.second.spriteChains;
										spriteChainVector.clear();
									}

									matEntry.enabled = false;
								}
							}
							pipelineEntry.enabled = false;
						}
					}

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

					layer.depthSortedMeshes.clear();
					layer.depthSortedMeshData.clear();
					layer.depthSortedSpriteData.clear();
					layer.depthSortedSprites.clear();
					layer.otherDrawables.clear();
					++it;
				}
			}
		}
	}

	/*!
	* \brief Sorts the object according to the viewer position, furthest to nearest
	*
	* \param viewer Viewer of the scene
	*/

	void ForwardRenderQueue::Sort(const AbstractViewer* viewer)
	{
		std::unordered_map<int, std::size_t> layers;
		for (int layer : m_renderLayers)
			layers.emplace(layer, layers.size());

		std::unordered_map<const Nz::MaterialPipeline*, std::size_t> pipelines;
		std::unordered_map<const Nz::Material*, std::size_t> materials;
		std::unordered_map<const Nz::Texture*, std::size_t> overlays;
		std::unordered_map<const Nz::UberShader*, std::size_t> shaders;
		std::unordered_map<const Nz::Texture*, std::size_t> textures;
		std::unordered_map<const Nz::VertexBuffer*, std::size_t> vertexBuffers;

		opaqueModels.Sort([&](const OpaqueModels& renderData)
		{
			// RQ index:
			// - Layer (4bits)
			// - Pipeline (8bits)
			// - Material (8bits)
			// - Shader? (8bits)
			// - Textures (8bits)
			// - Buffers (8bits)
			// - Scissor (4bits)
			// - Depth? (16bits)

			auto GetOrInsert = [](auto& container, auto&& value)
			{
				return container.emplace(value, container.size()).first->second;
			};

			Nz::UInt64 layerIndex = layers[renderData.layerIndex];
			Nz::UInt64 pipelineIndex = GetOrInsert(pipelines, renderData.material->GetPipeline());
			Nz::UInt64 materialIndex = GetOrInsert(materials, renderData.material);
			Nz::UInt64 shaderIndex = GetOrInsert(shaders, renderData.material->GetShader());
			Nz::UInt64 textureIndex = GetOrInsert(textures, renderData.material->GetDiffuseMap());
			Nz::UInt64 bufferIndex = GetOrInsert(vertexBuffers, renderData.meshData.vertexBuffer);
			Nz::UInt64 scissorIndex = 0; //< TODO
			Nz::UInt64 depthIndex = 0; //< TODO

			Nz::UInt64 index = (layerIndex    & 0xF0)   << 60 |
			                   (pipelineIndex & 0xFF)   << 52 |
			                   (materialIndex & 0xFF)   << 44 |
			                   (shaderIndex   & 0xFF)   << 36 |
			                   (textureIndex  & 0xFF)   << 28 |
			                   (bufferIndex   & 0xFF)   << 20 |
			                   (scissorIndex  & 0xF0)   << 16 |
			                   (depthIndex    & 0xFFFF) <<  0;
			return index;
		});
		
		basicSprites.Sort([&](const BasicSprites& vertices)
		{
			// RQ index:
			// - Layer (4bits)
			// - Pipeline (8bits)
			// - Material (8bits)
			// - Shader? (8bits)
			// - Textures (8bits)
			// - Overlay (8bits)
			// - Scissor (4bits)
			// - Depth? (16bits)

			auto GetOrInsert = [](auto& container, auto&& value)
			{
				return container.emplace(value, container.size()).first->second;
			};

			Nz::UInt64 layerIndex = layers[vertices.layerIndex];
			Nz::UInt64 pipelineIndex = GetOrInsert(pipelines, vertices.material->GetPipeline());
			Nz::UInt64 materialIndex = GetOrInsert(materials, vertices.material);
			Nz::UInt64 shaderIndex = GetOrInsert(shaders, vertices.material->GetShader());
			Nz::UInt64 textureIndex = GetOrInsert(textures, vertices.material->GetDiffuseMap());
			Nz::UInt64 overlayIndex = GetOrInsert(overlays, vertices.overlay);
			Nz::UInt64 scissorIndex = 0; //< TODO
			Nz::UInt64 depthIndex = 0; //< TODO

			Nz::UInt64 index = (layerIndex    & 0xF0)   << 60 |
			                   (pipelineIndex & 0xFF)   << 52 |
			                   (materialIndex & 0xFF)   << 44 |
			                   (shaderIndex   & 0xFF)   << 36 |
			                   (textureIndex  & 0xFF)   << 28 |
			                   (overlayIndex  & 0xFF)   << 20 |
			                   (scissorIndex  & 0xF0)   << 16 |
			                   (depthIndex    & 0xFFFF) <<  0;

			return index;
		});

		if (viewer->GetProjectionType() == ProjectionType_Orthogonal)
			SortForOrthographic(viewer);
		else
			SortForPerspective(viewer);
	}

	/*!
	* \brief Gets the billboard data
	* \return Pointer to the data of the billboards
	*
	* \param renderOrder Order of rendering
	* \param material Material of the billboard
	*/

	ForwardRenderQueue::BillboardData* ForwardRenderQueue::GetBillboardData(int renderOrder, const Material* material, unsigned int count)
	{
		auto& billboards = GetLayer(renderOrder).billboards;

		const MaterialPipeline* materialPipeline = material->GetPipeline();

		auto pipelineIt = billboards.find(materialPipeline);
		if (pipelineIt == billboards.end())
		{
			BatchedBillboardPipelineEntry pipelineEntry;
			pipelineIt = billboards.insert(BillboardPipelineBatches::value_type(materialPipeline, std::move(pipelineEntry))).first;
		}
		BatchedBillboardPipelineEntry& pipelineEntry = pipelineIt->second;
		pipelineEntry.enabled = true;

		BatchedBillboardContainer& materialMap = pipelineEntry.materialMap;

		auto it = materialMap.find(material);
		if (it == materialMap.end())
		{
			BatchedBillboardEntry entry;
			entry.materialReleaseSlot.Connect(material->OnMaterialRelease, this, &ForwardRenderQueue::OnMaterialInvalidation);

			it = materialMap.insert(BatchedBillboardContainer::value_type(material, std::move(entry))).first;
		}

		BatchedBillboardEntry& entry = it->second;

		auto& billboardVector = entry.billboards;
		std::size_t prevSize = billboardVector.size();
		billboardVector.resize(prevSize + count);

		return &billboardVector[prevSize];
	}

	/*!
	* \brief Gets the ith layer
	* \return Reference to the ith layer for the queue
	*
	* \param i Index of the layer
	*/

	ForwardRenderQueue::Layer& ForwardRenderQueue::GetLayer(int i)
	{
		auto it = layers.find(i);
		if (it == layers.end())
			it = layers.insert(std::make_pair(i, Layer())).first;

		Layer& layer = it->second;
		layer.clearCount = 0;

		return layer;
	}

	void ForwardRenderQueue::SortBillboards(Layer& layer, const Planef& nearPlane)
	{
		for (auto& pipelinePair : layer.billboards)
		{
			for (auto& matPair : pipelinePair.second.materialMap)
			{
				const Material* mat = matPair.first;

				if (mat->IsDepthSortingEnabled())
				{
					BatchedBillboardEntry& entry = matPair.second;
					auto& billboardVector = entry.billboards;

					std::sort(billboardVector.begin(), billboardVector.end(), [&nearPlane] (const BillboardData& data1, const BillboardData& data2)
					{
						return nearPlane.Distance(data1.center) > nearPlane.Distance(data2.center);
					});
				}
			}
		}
	}

	void ForwardRenderQueue::SortForOrthographic(const AbstractViewer * viewer)
	{
		Planef nearPlane = viewer->GetFrustum().GetPlane(FrustumPlane_Near);

		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			std::sort(layer.depthSortedMeshes.begin(), layer.depthSortedMeshes.end(), [&layer, &nearPlane] (std::size_t index1, std::size_t index2)
			{
				const Spheref& sphere1 = layer.depthSortedMeshData[index1].obbSphere;
				const Spheref& sphere2 = layer.depthSortedMeshData[index2].obbSphere;

				return nearPlane.Distance(sphere1.GetPosition()) < nearPlane.Distance(sphere2.GetPosition());
			});

			std::sort(layer.depthSortedSprites.begin(), layer.depthSortedSprites.end(), [&layer, &nearPlane] (std::size_t index1, std::size_t index2)
			{
				const Vector3f& pos1 = layer.depthSortedSpriteData[index1].vertices[0].position;
				const Vector3f& pos2 = layer.depthSortedSpriteData[index2].vertices[0].position;

				return nearPlane.Distance(pos1) < nearPlane.Distance(pos2);
			});

			SortBillboards(layer, nearPlane);
		}
	}

	void ForwardRenderQueue::SortForPerspective(const AbstractViewer* viewer)
	{
		Planef nearPlane = viewer->GetFrustum().GetPlane(FrustumPlane_Near);
		Vector3f viewerPos = viewer->GetEyePosition();

		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			std::sort(layer.depthSortedMeshes.begin(), layer.depthSortedMeshes.end(), [&layer, &viewerPos] (std::size_t index1, std::size_t index2)
			{
				const Spheref& sphere1 = layer.depthSortedMeshData[index1].obbSphere;
				const Spheref& sphere2 = layer.depthSortedMeshData[index2].obbSphere;

				return viewerPos.SquaredDistance(sphere1.GetPosition()) > viewerPos.SquaredDistance(sphere2.GetPosition());
			});

			std::sort(layer.depthSortedSprites.begin(), layer.depthSortedSprites.end(), [&layer, &viewerPos] (std::size_t index1, std::size_t index2)
			{
				const Vector3f& pos1 = layer.depthSortedSpriteData[index1].vertices[0].position;
				const Vector3f& pos2 = layer.depthSortedSpriteData[index2].vertices[0].position;

				return viewerPos.SquaredDistance(pos1) > viewerPos.SquaredDistance(pos2);
			});

			SortBillboards(layer, nearPlane);
		}
	}

	/*!
	* \brief Handle the invalidation of an index buffer
	*
	* \param indexBuffer Index buffer being invalidated
	*/
	void ForwardRenderQueue::OnIndexBufferInvalidation(const IndexBuffer* indexBuffer)
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

	void ForwardRenderQueue::OnMaterialInvalidation(const Material* material)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;

			for (auto& pipelineEntry : layer.opaqueSprites)
				pipelineEntry.second.materialMap.erase(material);

			for (auto& pipelineEntry : layer.billboards)
				pipelineEntry.second.materialMap.erase(material);

			for (auto& pipelineEntry : layer.opaqueModels)
				pipelineEntry.second.materialMap.erase(material);
		}
	}

	/*!
	* \brief Handle the invalidation of a texture
	*
	* \param texture Texture being invalidated
	*/

	void ForwardRenderQueue::OnTextureInvalidation(const Texture* texture)
	{
		for (auto& pair : layers)
		{
			Layer& layer = pair.second;
			for (auto& pipelineEntry : layer.opaqueSprites)
			{
				for (auto& materialEntry : pipelineEntry.second.materialMap)
					materialEntry.second.overlayMap.erase(texture);
			}
		}
	}

	/*!
	* \brief Handle the invalidation of a vertex buffer
	*
	* \param vertexBuffer Vertex buffer being invalidated
	*/

	void ForwardRenderQueue::OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer)
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

	bool ForwardRenderQueue::MaterialComparator::operator()(const Material* mat1, const Material* mat2) const
	{
		const Texture* diffuseMap1 = mat1->GetDiffuseMap();
		const Texture* diffuseMap2 = mat2->GetDiffuseMap();
		if (diffuseMap1 != diffuseMap2)
			return diffuseMap1 < diffuseMap2;

		return mat1 < mat2;
	}

	bool ForwardRenderQueue::MaterialPipelineComparator::operator()(const MaterialPipeline* pipeline1, const MaterialPipeline* pipeline2) const
	{
		const Shader* shader1 = pipeline1->GetInstance().renderPipeline.GetInfo().shader;
		const Shader* shader2 = pipeline2->GetInstance().renderPipeline.GetInfo().shader;
		if (shader1 != shader2)
			return shader1 < shader2;

		return pipeline1 < pipeline2;
	}

	/*!
	* \brief Functor to compare two mesh data
	* \return true If first mesh is "smaller" than the second one
	*
	* \param data1 First mesh to compare
	* \param data2 Second mesh to compare
	*/

	bool ForwardRenderQueue::MeshDataComparator::operator()(const MeshData& data1, const MeshData& data2) const
	{
		const Buffer* buffer1;
		const Buffer* buffer2;

		buffer1 = (data1.indexBuffer) ? data1.indexBuffer->GetBuffer() : nullptr;
		buffer2 = (data2.indexBuffer) ? data2.indexBuffer->GetBuffer() : nullptr;
		if (buffer1 != buffer2)
			return buffer1 < buffer2;

		buffer1 = data1.vertexBuffer->GetBuffer();
		buffer2 = data2.vertexBuffer->GetBuffer();
		if (buffer1 != buffer2)
			return buffer1 < buffer2;

		return data1.primitiveMode < data2.primitiveMode;
	}
}
