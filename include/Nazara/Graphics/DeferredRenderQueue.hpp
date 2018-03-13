// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERQUEUE_HPP
#define NAZARA_DEFERREDRENDERQUEUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredRenderQueue : public AbstractRenderQueue
	{
		public:
			DeferredRenderQueue(ForwardRenderQueue* forwardQueue);
			~DeferredRenderQueue() = default;

			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, std::size_t billboardCount, const Recti& scissorRect, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddDrawable(int renderOrder, const Drawable* drawable) override;
			void AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect) override;
			void AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay = nullptr) override;

			void Clear(bool fully = false) override;

			struct MeshInstanceEntry
			{
				NazaraSlot(IndexBuffer, OnIndexBufferRelease, indexBufferReleaseSlot);
				NazaraSlot(VertexBuffer, OnVertexBufferRelease, vertexBufferReleaseSlot);

				std::vector<Matrix4f> instances;
			};

			typedef std::map<MeshData, MeshInstanceEntry, ForwardRenderQueue::MeshDataComparator> MeshInstanceContainer;

			struct BatchedModelEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				MeshInstanceContainer meshMap;
				bool enabled = false;
			};

			typedef std::map<const Material*, BatchedModelEntry, ForwardRenderQueue::MaterialComparator> MeshMaterialBatches;

			struct BatchedMaterialEntry
			{
				std::size_t maxInstanceCount = 0;
				MeshMaterialBatches materialMap;
			};

			typedef std::map<const MaterialPipeline*, BatchedMaterialEntry, ForwardRenderQueue::MaterialPipelineComparator> MeshPipelineBatches;

			struct Layer
			{
				MeshPipelineBatches opaqueModels;
				unsigned int clearCount = 0;
			};

			std::map<int, Layer> layers;

		private:
			Layer& GetLayer(unsigned int i); ///TODO: Inline

			ForwardRenderQueue* m_forwardQueue;

			void OnIndexBufferInvalidation(const IndexBuffer* indexBuffer);
			void OnMaterialInvalidation(const Material* material);
			void OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer);
	};
}

#endif // NAZARA_DEFERREDRENDERQUEUE_HPP
