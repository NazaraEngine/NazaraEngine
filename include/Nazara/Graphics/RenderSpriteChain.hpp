// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSUBMESH_HPP
#define NAZARA_RENDERSUBMESH_HPP

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
	class VertexDeclaration;

	class RenderSpriteChain : public RenderElement
	{
		public:
			inline RenderSpriteChain(int renderLayer, std::shared_ptr<RenderPipeline> renderPipeline, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<Texture> textureOverlay, std::size_t spriteCount, const void* spriteData, const ShaderBinding& materialBinding, const WorldInstance& worldInstance, const MaterialPassFlags& matFlags);
			~RenderSpriteChain() = default;

			inline UInt64 ComputeSortingScore(const Nz::Frustumf& frustum, const RenderQueueRegistry& registry) const override;

			inline const ShaderBinding& GetInstanceBinding() const;
			inline const ShaderBinding& GetMaterialBinding() const;
			inline const RenderPipeline* GetRenderPipeline() const;
			inline std::size_t GetSpriteCount() const;
			inline const void* GetSpriteData() const;
			inline const Texture* GetTextureOverlay() const;
			inline const VertexDeclaration* GetVertexDeclaration() const;

			inline void Register(RenderQueueRegistry& registry) const override;

		private:
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::shared_ptr<VertexDeclaration> m_vertexDeclaration;
			std::shared_ptr<Texture> m_textureOverlay;
			std::size_t m_spriteCount;
			const void* m_spriteData;
			MaterialPassFlags m_matFlags;
			const ShaderBinding& m_materialBinding;
			const WorldInstance& m_worldInstance;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/RenderSpriteChain.inl>

#endif
