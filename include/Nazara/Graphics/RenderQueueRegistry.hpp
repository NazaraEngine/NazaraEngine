// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
#define NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Thirdparty/ankerl/unordered_dense.h>
#include <set>

namespace Nz
{
	class MaterialInstance;
	class RenderBuffer;
	class RenderPipeline;
	class Skeleton;
	class VertexDeclaration;

	class RenderQueueRegistry
	{
		public:
			RenderQueueRegistry() = default;
			~RenderQueueRegistry() = default;

			inline void Clear();

			inline std::size_t FetchLayerIndex(int renderLayer) const;
			inline std::size_t FetchMaterialInstanceIndex(const MaterialInstance* materialInstance) const;
			inline std::size_t FetchPipelineIndex(const RenderPipeline* pipeline) const;
			inline std::size_t FetchSkeletonIndex(const Skeleton* skeleton) const;
			inline std::size_t FetchVertexBuffer(const RenderBuffer* vertexBuffer) const;
			inline std::size_t FetchVertexDeclaration(const VertexDeclaration* vertexDeclaration) const;

			inline void Finalize();

			inline void RegisterLayer(int renderLayer);
			inline void RegisterMaterialInstance(const MaterialInstance* materialInstance);
			inline void RegisterPipeline(const RenderPipeline* pipeline);
			inline void RegisterSkeleton(const Skeleton* skeleton);
			inline void RegisterVertexBuffer(const RenderBuffer* vertexBuffer);
			inline void RegisterVertexDeclaration(const VertexDeclaration* vertexDeclaration);

		private:
			std::set<int> m_renderLayers;
			ankerl::unordered_dense::map<int, std::size_t> m_renderLayerRegistry;
			ankerl::unordered_dense::map<const MaterialInstance*, std::size_t> m_materialPassRegistry;
			ankerl::unordered_dense::map<const RenderPipeline*, std::size_t> m_pipelineRegistry;
			ankerl::unordered_dense::map<const RenderBuffer*, std::size_t> m_vertexBufferRegistry;
			ankerl::unordered_dense::map<const Skeleton*, std::size_t> m_skeletonRegistry;
			ankerl::unordered_dense::map<const VertexDeclaration*, std::size_t> m_vertexDeclarationRegistry;
	};
}

#include <Nazara/Graphics/RenderQueueRegistry.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
