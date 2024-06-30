// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <NazaraUtils/CallOnExit.hpp>

namespace Nz
{
	TextSprite::TextSprite(std::shared_ptr<MaterialInstance> material) :
	InstancedRenderable(),
	m_material(std::move(material))
	{
		if (!m_material)
			m_material = MaterialInstance::GetDefault(MaterialType::Basic, MaterialInstancePreset::Transparent);
	}

	void TextSprite::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
	{
		const auto& materialPipeline = m_material->GetPipeline(passIndex);
		if (!materialPipeline)
			return;

		MaterialPassFlags passFlags = m_material->GetPassFlags(passIndex);

		const std::shared_ptr<VertexDeclaration>& vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ_Color_UV);

		RenderPipelineInfo::VertexBufferData vertexBufferData = {
			0,
			vertexDeclaration
		};
		const auto& renderPipeline = materialPipeline->GetRenderPipeline(&vertexBufferData, 1);

		for (auto& pair : m_renderInfos)
		{
			const RenderKey& key = pair.first;
			RenderIndices& indices = pair.second;

			if (indices.count > 0)
				elements.emplace_back(registry.AllocateElement<RenderSpriteChain>(GetRenderLayer() + key.renderOrder, m_material, passFlags, renderPipeline, *elementData.worldInstance, vertexDeclaration, indices.textureAsset, indices.count, &m_vertices[indices.first * 4], *elementData.scissorBox));
		}
	}

	const std::shared_ptr<MaterialInstance>& TextSprite::GetMaterial(std::size_t i) const
	{
		assert(i == 0);
		NazaraUnused(i);

		return m_material;
	}

	std::size_t TextSprite::GetMaterialCount() const
	{
		return 1;
	}

	void TextSprite::Update(const AbstractTextDrawer& drawer, float scale)
	{
		CallOnExit clearOnFail([this]()
		{
			Clear();
		});

		// Mark every atlas as unused...
		for (auto& pair : m_atlases)
			pair.second.used = false;

		// ... until they are marked as used by the drawer
		std::size_t fontCount = drawer.GetFontCount();
		for (std::size_t i = 0; i < fontCount; ++i)
		{
			Font& font = *drawer.GetFont(i);
			const AbstractAtlas* atlas = font.GetAtlas().get();
			NazaraAssert(atlas->GetStorage() == DataStorage::Hardware, "Font uses a non-hardware atlas which cannot be used by text sprites");

			auto it = m_atlases.find(atlas);
			if (it == m_atlases.end())
			{
				it = m_atlases.emplace(std::make_pair(atlas, AtlasSlots())).first;
				AtlasSlots& atlasSlots = it->second;

				atlasSlots.clearSlot.Connect(atlas->OnAtlasCleared, this, &TextSprite::OnAtlasInvalidated);
				atlasSlots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &TextSprite::OnAtlasLayerChange);
				atlasSlots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &TextSprite::OnAtlasInvalidated);
			}

			it->second.used = true;
		}

		// Remove unused atlas slots
		auto atlasIt = m_atlases.begin();
		while (atlasIt != m_atlases.end())
		{
			if (!atlasIt->second.used)
				m_atlases.erase(atlasIt++);
			else
				++atlasIt;
		}

		std::size_t spriteCount = drawer.GetSpriteCount();
		const AbstractTextDrawer::Sprite* sprites = drawer.GetSprites();

		// Reset glyph count for every texture to zero
		for (auto& pair : m_renderInfos)
			pair.second.count = 0;

		// Count glyph count for each texture
		RenderKey lastRenderKey{ nullptr, 0 };
		unsigned int* count = nullptr;

		// Iterate over visible (non-space) glyphs
		std::size_t visibleGlyphCount = 0;
		for (std::size_t i = 0; i < spriteCount; ++i)
		{
			const AbstractTextDrawer::Sprite& sprite = sprites[i];

			Texture* texture = static_cast<Texture*>(sprite.atlas);
			RenderKey renderKey{ texture, sprite.renderOrder };
			if (lastRenderKey != renderKey)
			{
				auto it = m_renderInfos.find(renderKey);
				if (it == m_renderInfos.end())
				{
					it = m_renderInfos.insert(std::make_pair(renderKey, RenderIndices{ 0U, 0U })).first;
					it->second.textureAsset = TextureAsset::CreateFromTexture(texture->shared_from_this());
				}

				count = &it->second.count;
				lastRenderKey = renderKey;
			}

			(*count)++;
			visibleGlyphCount++;
		}

		m_vertices.resize(visibleGlyphCount * 4);

		// Attributes indices and reinitialize glyph count to zero to use it as a counter in the next loop
		// This is because the 1st glyph can use texture A, the 2nd glyph can use texture B and the 3th glyph C can use texture A again
		// so we need a counter to know where to write informations
		// also remove unused render infos
		unsigned int index = 0;
		auto infoIt = m_renderInfos.begin();
		while (infoIt != m_renderInfos.end())
		{
			RenderIndices& indices = infoIt->second;
			if (indices.count == 0)
				infoIt = m_renderInfos.erase(infoIt); //< No glyph uses this texture, remove from indices
			else
			{
				indices.first = index;

				index += indices.count;
				indices.count = 0;
				++infoIt;
			}
		}

		Rectf bounds = drawer.GetBounds();

		lastRenderKey = { nullptr, 0 };
		RenderIndices* indices = nullptr;
		for (unsigned int i = 0; i < spriteCount; ++i)
		{
			const AbstractTextDrawer::Sprite& sprite = sprites[i];

			Texture* texture = static_cast<Texture*>(sprite.atlas);
			RenderKey renderKey{ texture, sprite.renderOrder };
			if (lastRenderKey != renderKey)
			{
				indices = &m_renderInfos[renderKey]; //< We changed texture, adjust the pointer
				lastRenderKey = renderKey;
			}

			// First, compute the uv coordinates from our atlas rect
			Vector2ui size(texture->GetSize());
			float invWidth = 1.f / size.x;
			float invHeight = 1.f / size.y;

			Rectf uvRect(sprite.atlasRect);
			uvRect.x *= invWidth;
			uvRect.y *= invHeight;
			uvRect.width *= invWidth;
			uvRect.height *= invHeight;

			// Our glyph may be flipped in the atlas, to render it correctly we need to change the uv coordinates accordingly
			constexpr RectCorner normalCorners[4] = { RectCorner::LeftTop, RectCorner::RightTop, RectCorner::LeftBottom, RectCorner::RightBottom };
			constexpr RectCorner flippedCorners[4] = { RectCorner::LeftBottom, RectCorner::LeftTop, RectCorner::RightBottom, RectCorner::RightTop };

			// Glyph are in clockwise order, but the sprite expects them as CCW
			std::size_t offset = indices->first + indices->count;
			offset *= 4;

			for (std::size_t cornerIndex : { 0, 2, 1, 3 })
			{
				// Set the position, color and UV of our vertices
				// Remember that indices->count is a counter here, not a count value
				m_vertices[offset].color = sprite.color;
				m_vertices[offset].position = sprite.corners[cornerIndex];
				m_vertices[offset].position.y = bounds.height - m_vertices[offset].position.y;
				m_vertices[offset].position *= scale;
				m_vertices[offset].uv = uvRect.GetCorner((sprite.flipped) ? flippedCorners[cornerIndex] : normalCorners[cornerIndex]);
				offset++;
			}

			// Increment the counter, go to next glyph
			indices->count++;
		}

		bounds.Scale(scale);

		UpdateAABB(bounds);
		OnElementInvalidated(this);

		clearOnFail.Reset();
	}

	/*!
	* \brief Handle the invalidation of an atlas
	*
	* \param atlas Atlas being invalidated
	*/
	void TextSprite::OnAtlasInvalidated(const AbstractAtlas* atlas)
	{
		assert(m_atlases.find(atlas) != m_atlases.end());

		NazaraWarningFmt("TextSprite {0} has been cleared because atlas {1} has been invalidated (cleared or released)", fmt::ptr(this), fmt::ptr(atlas));
		Clear();
	}

	/*!
	* \brief Handle the size change of an atlas layer
	*
	* \param atlas Atlas being invalidated
	* \param oldLayer Pointer to the previous layer
	* \param newLayer Pointer to the new layer
	*/
	void TextSprite::OnAtlasLayerChange([[maybe_unused]] const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		assert(m_atlases.find(atlas) != m_atlases.end());

		if (!oldLayer)
			return;

		assert(newLayer);

		// The texture of an atlas have just been recreated (size change)
		// we have to adjust the coordinates of the texture and the rendering texture
		Texture* oldTexture = static_cast<Texture*>(oldLayer);
		Texture* newTexture = static_cast<Texture*>(newLayer);

		Vector2ui oldSize(oldTexture->GetSize());
		Vector2ui newSize(newTexture->GetSize());
		Vector2f scale = Vector2f(oldSize) / Vector2f(newSize);

		// Rebuild new render infos with adjusted texture
		decltype(m_renderInfos) newRenderInfos;

		for (auto&& [renderKey, indices] : m_renderInfos)
		{
			if (renderKey.texture != oldTexture)
			{
				newRenderInfos.emplace(renderKey, indices);
				continue;
			}

			// Adjust texture coordinates by size ratio
			SparsePtr<Vector2f> texCoordPtr(&m_vertices[indices.first].uv, sizeof(VertexStruct_XY_Color_UV));
			for (unsigned int i = 0; i < indices.count; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
					m_vertices[i * 4 + j].uv *= scale;
			}

			indices.textureAsset = TextureAsset::CreateFromTexture(newTexture->shared_from_this());
			newRenderInfos.emplace(RenderKey{ newTexture, renderKey.renderOrder }, indices);
		}

		m_renderInfos = std::move(newRenderInfos);

		OnElementInvalidated(this);
	}
}
