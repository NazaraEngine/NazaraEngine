// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP
#define NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class AbstractBuffer;
	class MaterialPass;
	class VertexDeclaration;
	class ViewerInstance;

	class RenderSpriteChain : public RenderElement
	{
		public:
			inline RenderSpriteChain(int renderLayer, std::shared_ptr<MaterialPass> materialPass, std::shared_ptr<RenderPipeline> renderPipeline, const WorldInstance& worldInstance, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<Texture> textureOverlay, std::size_t spriteCount, const void* spriteData);
			~RenderSpriteChain() = default;

			inline UInt64 ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const override;

			inline const MaterialPass& GetMaterialPass() const;
			inline const RenderPipeline& GetRenderPipeline() const;
			inline std::size_t GetSpriteCount() const;
			inline const void* GetSpriteData() const;
			inline const Texture* GetTextureOverlay() const;
			inline const VertexDeclaration* GetVertexDeclaration() const;
			inline const WorldInstance& GetWorldInstance() const;

			inline void Register(RenderQueueRegistry& registry) const override;

		private:
			std::shared_ptr<MaterialPass> m_materialPass;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::shared_ptr<VertexDeclaration> m_vertexDeclaration;
			std::shared_ptr<Texture> m_textureOverlay;
			std::size_t m_spriteCount;
			const void* m_spriteData;
			const WorldInstance& m_worldInstance;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/RenderSpriteChain.inl>

#endif // NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP
