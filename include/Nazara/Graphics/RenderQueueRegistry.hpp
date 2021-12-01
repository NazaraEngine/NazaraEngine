// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
#define NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Thirdparty/robin_hood/robin_hood.h>
#include <set>

namespace Nz
{
	class AbstractBuffer;
	class MaterialPass;
	class RenderPipeline;
	class VertexDeclaration;

	class RenderQueueRegistry
	{
		public:
			RenderQueueRegistry() = default;
			~RenderQueueRegistry() = default;

			inline void Clear();

			inline std::size_t FetchLayerIndex(int renderLayer) const;
			inline std::size_t FetchMaterialPassIndex(const MaterialPass* materialPass) const;
			inline std::size_t FetchPipelineIndex(const RenderPipeline* pipeline) const;
			inline std::size_t FetchVertexBuffer(const AbstractBuffer* vertexBuffer) const;
			inline std::size_t FetchVertexDeclaration(const VertexDeclaration* vertexDeclaration) const;

			inline void Finalize();

			inline void RegisterLayer(int renderLayer);
			inline void RegisterMaterialPass(const MaterialPass* materialPass);
			inline void RegisterPipeline(const RenderPipeline* pipeline);
			inline void RegisterVertexBuffer(const AbstractBuffer* vertexBuffer);
			inline void RegisterVertexDeclaration(const VertexDeclaration* vertexDeclaration);

		private:
			std::set<int> m_renderLayers;
			robin_hood::unordered_map<int, std::size_t> m_renderLayerRegistry;
			robin_hood::unordered_map<const MaterialPass*, std::size_t> m_materialPassRegistry;
			robin_hood::unordered_map<const RenderPipeline*, std::size_t> m_pipelineRegistry;
			robin_hood::unordered_map<const AbstractBuffer*, std::size_t> m_vertexBufferRegistry;
			robin_hood::unordered_map<const VertexDeclaration*, std::size_t> m_vertexDeclarationRegistry;
	};
}

#include <Nazara/Graphics/RenderQueueRegistry.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
