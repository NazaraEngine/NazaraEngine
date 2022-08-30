// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TEXTSPRITE_HPP
#define NAZARA_GRAPHICS_TEXTSPRITE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

namespace Nz
{
	class AbstractTextDrawer;

	class NAZARA_GRAPHICS_API TextSprite : public InstancedRenderable
	{
		public:
			TextSprite(std::shared_ptr<Material> material);
			TextSprite(const TextSprite&) = delete;
			TextSprite(TextSprite&&) noexcept = default;
			~TextSprite() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline void Clear();

			const std::shared_ptr<Material>& GetMaterial(std::size_t i = 0) const override;
			std::size_t GetMaterialCount() const override;

			inline void SetMaterial(std::shared_ptr<Material> material);

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

			struct RenderData
			{

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
			};

			struct HashRenderKey
			{
				std::size_t operator()(const RenderKey& key) const
				{
					// Since renderOrder will be very small, this will be enough
					return std::hash<Texture*>()(key.texture) + key.renderOrder;
				}
			};

			struct RenderIndices
			{
				unsigned int first;
				unsigned int count;
			};

			std::unordered_map<const AbstractAtlas*, AtlasSlots> m_atlases;
			mutable std::unordered_map<RenderKey, RenderIndices, HashRenderKey> m_renderInfos;
			std::shared_ptr<Material> m_material;
			std::vector<RenderData> m_data;
			std::vector<VertexStruct_XYZ_Color_UV> m_vertices;
			Recti m_scissorBox;
	};
}

#include <Nazara/Graphics/TextSprite.inl>

#endif // NAZARA_GRAPHICS_TEXTSPRITE_HPP
