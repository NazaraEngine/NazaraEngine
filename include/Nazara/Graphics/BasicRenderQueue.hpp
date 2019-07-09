// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BASICRENDERQUEUE_HPP
#define NAZARA_BASICRENDERQUEUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class AbstractViewer;

	class NAZARA_GRAPHICS_API BasicRenderQueue : public AbstractRenderQueue
	{
		friend class ForwardRenderTechnique;

		public:
			struct BillboardData;

			BasicRenderQueue() = default;
			~BasicRenderQueue() = default;

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

			inline const BillboardData* GetBillboardData(std::size_t billboardIndex) const;

			void Sort(const AbstractViewer* viewer);

			struct BillboardData
			{
				Color color;
				Vector3f center;
				Vector2f size;
				Vector2f sinCos;
			};

			struct Billboard
			{
				int layerIndex;
				MovablePtr<const Nz::Material> material;
				Nz::Recti scissorRect;
				BillboardData data;
			};

			struct BillboardChain
			{
				int layerIndex;
				MovablePtr<const Nz::Material> material;
				Nz::Recti scissorRect;
				std::size_t billboardCount;
				std::size_t billboardIndex;
			};

			RenderQueue<BillboardChain> billboards;
			RenderQueue<Billboard> depthSortedBillboards;

			struct CustomDrawable
			{
				int layerIndex;
				MovablePtr<const Drawable> drawable;
			};

			RenderQueue<CustomDrawable> customDrawables;

			struct Model
			{
				int layerIndex;
				MeshData meshData;
				MovablePtr<const Nz::Material> material;
				Nz::Matrix4f matrix;
				Nz::Recti scissorRect;
				Nz::Spheref obbSphere;
			};

			RenderQueue<Model> models;
			RenderQueue<Model> depthSortedModels;

			struct SpriteChain
			{
				int layerIndex;
				std::size_t spriteCount;
				MovablePtr<const Material> material;
				MovablePtr<const Texture> overlay;
				MovablePtr<const VertexStruct_XYZ_Color_UV> vertices;
				Nz::Recti scissorRect;
			};

			RenderQueue<SpriteChain> basicSprites;
			RenderQueue<SpriteChain> depthSortedSprites;

		private:
			inline Color ComputeColor(float alpha);
			inline Vector2f ComputeSinCos(float angle);
			inline Vector2f ComputeSize(float size);

			inline void RegisterLayer(int layerIndex);

			std::unordered_map<const MaterialPipeline*, std::size_t> m_pipelineCache;
			std::unordered_map<const Material*, std::size_t> m_materialCache;
			std::unordered_map<const Texture*, std::size_t> m_overlayCache;
			std::unordered_map<const UberShader*, std::size_t> m_shaderCache;
			std::unordered_map<const Texture*, std::size_t> m_textureCache;
			std::unordered_map<const VertexBuffer*, std::size_t> m_vertexBufferCache;
			std::unordered_map<int, std::size_t> m_layerCache;

			std::vector<BillboardData> m_billboards;
			std::vector<int> m_renderLayers;
	};
}

#include <Nazara/Graphics/BasicRenderQueue.inl>

#endif // NAZARA_BASICRENDERQUEUE_HPP
