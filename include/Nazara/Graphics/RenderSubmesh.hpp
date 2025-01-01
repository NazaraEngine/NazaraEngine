// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERSUBMESH_HPP
#define NAZARA_GRAPHICS_RENDERSUBMESH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class MaterialInstance;
	class RenderPipeline;
	class ShaderBinding;

	class RenderSubmesh : public RenderElement
	{
		public:
			inline RenderSubmesh(int renderLayer, std::shared_ptr<MaterialInstance> materialInstance, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, const WorldInstance& worldInstance, const SkeletonInstance* skeletonInstance, std::size_t indexCount, IndexType indexType, std::shared_ptr<RenderBuffer> indexBuffer, std::shared_ptr<RenderBuffer> vertexBuffer, const Recti& scissorBox);
			~RenderSubmesh() = default;

			inline UInt64 ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const override;

			inline const RenderBuffer* GetIndexBuffer() const;
			inline std::size_t GetIndexCount() const;
			inline IndexType GetIndexType() const;
			inline const MaterialInstance& GetMaterialInstance() const;
			inline const RenderPipeline* GetRenderPipeline() const;
			inline const Recti& GetScissorBox() const;
			inline const SkeletonInstance* GetSkeletonInstance() const;
			inline const RenderBuffer* GetVertexBuffer() const;
			inline const WorldInstance& GetWorldInstance() const;

			inline void Register(RenderQueueRegistry& registry) const override;

			static constexpr BasicRenderElement ElementType = BasicRenderElement::Submesh;

		private:
			std::shared_ptr<RenderBuffer> m_indexBuffer;
			std::shared_ptr<RenderBuffer> m_vertexBuffer;
			std::shared_ptr<MaterialInstance> m_materialInstance;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::size_t m_indexCount;
			const SkeletonInstance* m_skeletonInstance;
			const WorldInstance& m_worldInstance;
			IndexType m_indexType;
			MaterialPassFlags m_materialFlags;
			Recti m_scissorBox;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/RenderSubmesh.inl>

#endif // NAZARA_GRAPHICS_RENDERSUBMESH_HPP
