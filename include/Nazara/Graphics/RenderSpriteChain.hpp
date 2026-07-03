// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP
#define NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class MaterialInstance;
	class TextureAsset;
	class VertexDeclaration;
	class ViewerInstance;

	class RenderSpriteChain : public RenderElement
	{
		public:
			inline RenderSpriteChain(int renderLayer, std::shared_ptr<MaterialInstance> materialInstance, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, UInt32 instanceIndex, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<TextureAsset> textureOverlay, std::size_t spriteCount, const void* spriteData, const Recti& scissorBox);
			~RenderSpriteChain() = default;

			inline UInt64 ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const override;

			inline UInt32 GetInstanceIndex() const;
			inline const MaterialInstance& GetMaterialInstance() const;
			inline const RenderPipeline& GetRenderPipeline() const;
			inline const Recti& GetScissorBox() const;
			inline std::size_t GetSpriteCount() const;
			inline const void* GetSpriteData() const;
			inline const TextureAsset* GetTextureOverlay() const;
			inline const VertexDeclaration* GetVertexDeclaration() const;

			inline void Register(RenderQueueRegistry& registry) const override;

			static constexpr BasicRenderElement ElementType = BasicRenderElement::SpriteChain;
			static constexpr Nz::UInt32 MaxSpritePerChain = 4 * 1024;

		private:
			std::shared_ptr<MaterialInstance> m_materialInstance;
			std::shared_ptr<RenderPipeline> m_renderPipeline;
			std::shared_ptr<VertexDeclaration> m_vertexDeclaration;
			std::shared_ptr<TextureAsset> m_textureOverlay;
			std::size_t m_spriteCount;
			const void* m_spriteData;
			MaterialPassFlags m_materialFlags;
			Recti m_scissorBox;
			UInt32 m_instanceIndex;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/RenderSpriteChain.inl>

#endif // NAZARA_GRAPHICS_RENDERSPRITECHAIN_HPP
