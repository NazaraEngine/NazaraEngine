// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>
#include <tuple>

namespace Nz
{
	class AbstractViewer;

	class NAZARA_GRAPHICS_API ForwardRenderQueue : public AbstractRenderQueue
	{
		friend class ForwardRenderTechnique;

		public:
			ForwardRenderQueue() = default;
			~ForwardRenderQueue() = default;

			void AddBillboard(int renderOrder, const Material* material, const Vector3f& position, const Vector2f& size, const Vector2f& sinCos = Vector2f(0.f, 1.f), const Color& color = Color::White) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) override;
			void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) override;
			void AddDrawable(int renderOrder, const Drawable* drawable) override;
			void AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix) override;
			void AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const Texture* overlay = nullptr) override;

			void Clear(bool fully = false) override;

			void Sort(const AbstractViewer* viewer);

			/// Billboards
			struct BillboardData
			{
				Color color;
				Vector3f center;
				Vector2f size;
				Vector2f sinCos;
			};

			struct BatchedBillboardComparator
			{
				bool operator()(const Material* mat1, const Material* mat2) const;
			};

			struct BatchedBillboardEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				std::vector<BillboardData> billboards;
			};

			typedef std::map<const Material*, BatchedBillboardEntry, BatchedBillboardComparator> BatchedBillboardContainer;

			/// Sprites
			struct SpriteChain_XYZ_Color_UV
			{
				const VertexStruct_XYZ_Color_UV* vertices;
				unsigned int spriteCount;
			};

			struct BatchedSpriteEntry
			{
				NazaraSlot(Texture, OnTextureRelease, textureReleaseSlot);

				std::vector<SpriteChain_XYZ_Color_UV> spriteChains;
			};

			struct BatchedSpriteMaterialComparator
			{
				bool operator()(const Material* mat1, const Material* mat2);
			};

			typedef std::map<const Texture*, BatchedSpriteEntry> BasicSpriteOverlayContainer;

			struct BatchedBasicSpriteEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				BasicSpriteOverlayContainer overlayMap;
				bool enabled = false;
			};

			typedef std::map<const Material*, BatchedBasicSpriteEntry> BasicSpriteBatches;

			/// Meshes
			struct MeshDataComparator
			{
				bool operator()(const MeshData& data1, const MeshData& data2) const;
			};

			struct MeshInstanceEntry
			{
				NazaraSlot(IndexBuffer, OnIndexBufferRelease, indexBufferReleaseSlot);
				NazaraSlot(VertexBuffer, OnVertexBufferRelease, vertexBufferReleaseSlot);

				std::vector<Matrix4f> instances;
				Spheref squaredBoundingSphere;
			};

			typedef std::map<MeshData, MeshInstanceEntry, MeshDataComparator> MeshInstanceContainer;

			struct BatchedModelMaterialComparator
			{
				bool operator()(const Material* mat1, const Material* mat2) const;
			};

			struct BatchedModelEntry
			{
				NazaraSlot(Material, OnMaterialRelease, materialReleaseSlot);

				MeshInstanceContainer meshMap;
				bool enabled = false;
				bool instancingEnabled = false;
			};

			typedef std::map<const Material*, BatchedModelEntry, BatchedModelMaterialComparator> ModelBatches;

			struct TransparentModelData
			{
				Matrix4f transformMatrix;
				MeshData meshData;
				Spheref squaredBoundingSphere;
				const Material* material;
			};

			typedef std::vector<unsigned int> TransparentModelContainer;

			struct Layer
			{
				BatchedBillboardContainer billboards;
				BasicSpriteBatches basicSprites;
				ModelBatches opaqueModels;
				TransparentModelContainer transparentModels;
				std::vector<TransparentModelData> transparentModelData;
				std::vector<const Drawable*> otherDrawables;
				unsigned int clearCount = 0;
			};

			std::map<int, Layer> layers;

		private:
			BillboardData* GetBillboardData(int renderOrder, const Material* material, unsigned int count);
			Layer& GetLayer(int i); ///TODO: Inline

			void OnIndexBufferInvalidation(const IndexBuffer* indexBuffer);
			void OnMaterialInvalidation(const Material* material);
			void OnTextureInvalidation(const Texture* texture);
			void OnVertexBufferInvalidation(const VertexBuffer* vertexBuffer);
	};
}

#endif // NAZARA_FORWARDRENDERQUEUE_HPP
