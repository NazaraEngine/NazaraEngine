// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TEXTSPRITE_HPP
#define NAZARA_GRAPHICS_TEXTSPRITE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractAtlas.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <array>
#include <map>

namespace Nz
{
	class AbstractTextDrawer;

	class NAZARA_GRAPHICS_API TextSprite : public InstancedRenderable
	{
		public:
			TextSprite(std::shared_ptr<MaterialInstance> material = {});
			TextSprite(const TextSprite&) = delete;
			TextSprite(TextSprite&&) noexcept = default;
			~TextSprite() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline void Clear();

			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t i = 0) const override;
			std::size_t GetMaterialCount() const override;

			inline void SetMaterial(std::shared_ptr<MaterialInstance> material);

			void Update(const AbstractTextDrawer& drawer, float scale = 1.f);

			TextSprite& operator=(const TextSprite&) = delete;
			TextSprite& operator=(TextSprite&&) noexcept = default;

		private:
			void OnAtlasInvalidated(const AbstractAtlas* atlas);
			void OnAtlasLayerChange(const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer);

			struct AtlasSlots
			{
				bool used;
				NazaraSlot(AbstractAtlas, OnAtlasCleared, clearSlot);
				NazaraSlot(AbstractAtlas, OnAtlasLayerChange, layerChangeSlot);
				NazaraSlot(AbstractAtlas, OnAtlasRelease, releaseSlot);
			};

			struct RenderKey
			{
				Texture* texture;
				int renderOrder;

				bool operator==(const RenderKey& rhs) const
				{
					return texture == rhs.texture && renderOrder == rhs.renderOrder;
				}

				bool operator!=(const RenderKey& rhs) const
				{
					return !operator==(rhs);
				}

				bool operator<(const RenderKey& rhs) const
				{
					if (renderOrder != rhs.renderOrder)
						return renderOrder < rhs.renderOrder;

					return texture < rhs.texture;
				}
			};

			struct RenderIndices
			{
				unsigned int first;
				unsigned int count;
			};

			mutable std::map<RenderKey, RenderIndices> m_renderInfos;
			std::unordered_map<const AbstractAtlas*, AtlasSlots> m_atlases;
			std::shared_ptr<MaterialInstance> m_material;
			std::vector<VertexStruct_XYZ_Color_UV> m_vertices;
	};
}

#include <Nazara/Graphics/TextSprite.inl>

#endif // NAZARA_GRAPHICS_TEXTSPRITE_HPP
