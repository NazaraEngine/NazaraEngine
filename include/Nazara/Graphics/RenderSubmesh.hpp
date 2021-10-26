// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERSUBMESH_HPP
#define NAZARA_GRAPHICS_RENDERSUBMESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class AbstractBuffer;
	class RenderPipeline;
	class ShaderBinding;

	class RenderSubmesh : public RenderElement
	{
		public:
			inline RenderSubmesh(int renderLayer, std::shared_ptr<RenderPipeline> renderPipeline, std::size_t indexCount, std::shared_ptr<AbstractBuffer> indexBuffer, std::shared_ptr<AbstractBuffer> vertexBuffer, const WorldInstance& worldInstance, const ShaderBinding& materialBinding, const MaterialPassFlags& matFlags);
			~RenderSubmesh() = default;

			inline UInt64 ComputeSortingScore(const Nz::Frustumf& frustum, const RenderQueueRegistry& registry) const override;

			inline const AbstractBuffer* GetIndexBuffer() const;
			inline std::size_t GetIndexCount() const;
			inline const RenderPipeline* GetRenderPipeline() const;
			inline const ShaderBinding& GetInstanceBinding() const;
			inline const ShaderBinding& GetMaterialBinding() const;
			inline const AbstractBuffer* GetVertexBuffer() const;

			inline void Register(RenderQueueRegistry& registry) const override;

		private:
			std::shared_ptr<AbstractBuffer> m_indexBuffer;
			std::shared_ptr<AbstractBuffer> m_vertexBuffer;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::size_t m_indexCount;
			MaterialPassFlags m_matFlags;
			const ShaderBinding& m_materialBinding;
			const WorldInstance& m_worldInstance;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/RenderSubmesh.inl>

#endif // NAZARA_GRAPHICS_RENDERSUBMESH_HPP
