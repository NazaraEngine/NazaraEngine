// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <memory>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::TextSprite
	* \brief Graphics class that represents the rendering of a sprite containing text
	*/

	/*!
	* \brief Adds the text to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/

	void TextSprite::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		if (!m_material)
			return;

		for (auto& pair : m_renderInfos)
		{
			Texture* overlay = pair.first;
			RenderIndices& indices = pair.second;

			if (indices.count > 0)
			{
				const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());
				renderQueue->AddSprites(instanceData.renderOrder, m_material, &vertices[indices.first * 4], indices.count, overlay);
			}
		}
	}

	/*!
	* \brief Updates the text
	*
	* \param drawer Drawer used to compose the text
	*
	* \remark Produces a NazaraAssert if atlas does not use a hardware storage
	*/

	void TextSprite::Update(const AbstractTextDrawer& drawer)
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
			Font* font = drawer.GetFont(i);
			const AbstractAtlas* atlas = font->GetAtlas().get();
			NazaraAssert(atlas->GetStorage() & DataStorage_Hardware, "Font uses a non-hardware atlas which cannot be used by text sprites");

			auto it = m_atlases.find(atlas);
			if (it == m_atlases.end())
			{
				it = m_atlases.insert(std::make_pair(atlas, AtlasSlots())).first;
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

		std::size_t glyphCount = drawer.GetGlyphCount();
		m_localVertices.resize(glyphCount * 4);

		// Reset glyph count for every texture to zero
		for (auto& pair : m_renderInfos)
			pair.second.count = 0;

		// Count glyph count for each texture
		Texture* lastTexture = nullptr;
		unsigned int* count = nullptr;
		for (std::size_t i = 0; i < glyphCount; ++i)
		{
			const AbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);
			if (!glyph.atlas)
				continue;

			Texture* texture = static_cast<Texture*>(glyph.atlas);
			if (lastTexture != texture)
			{
				auto it = m_renderInfos.find(texture);
				if (it == m_renderInfos.end())
					it = m_renderInfos.insert(std::make_pair(texture, RenderIndices{0U, 0U})).first;

				count = &it->second.count;
				lastTexture = texture;
			}

			(*count)++;
		}

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
				m_renderInfos.erase(infoIt++); //< No glyph uses this texture, remove from indices
			else
			{
				indices.first = index;

				index += indices.count;
				indices.count = 0;
				++infoIt;
			}
		}

		lastTexture = nullptr;
		RenderIndices* indices = nullptr;
		for (unsigned int i = 0; i < glyphCount; ++i)
		{
			const AbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);
			if (!glyph.atlas)
				continue;

			Texture* texture = static_cast<Texture*>(glyph.atlas);
			if (lastTexture != texture)
			{
				indices = &m_renderInfos[texture]; //< We changed texture, adjust the pointer
				lastTexture = texture;
			}

			// First, compute the uv coordinates from our atlas rect
			Vector2ui size(texture->GetSize());
			float invWidth = 1.f / size.x;
			float invHeight = 1.f / size.y;

			Rectf uvRect(glyph.atlasRect);
			uvRect.x *= invWidth;
			uvRect.y *= invHeight;
			uvRect.width *= invWidth;
			uvRect.height *= invHeight;

			// Our glyph may be flipped in the atlas, to render it correctly we need to change the uv coordinates accordingly
			const RectCorner normalCorners[4] = {RectCorner_LeftTop, RectCorner_RightTop, RectCorner_LeftBottom, RectCorner_RightBottom};
			const RectCorner flippedCorners[4] = {RectCorner_LeftBottom, RectCorner_LeftTop, RectCorner_RightBottom, RectCorner_RightTop};

			// Set the position, color and UV of our vertices
			for (unsigned int j = 0; j < 4; ++j)
			{
				// Remember that indices->count is a counter here, not a count value
				m_localVertices[indices->count * 4 + j].color = glyph.color;
				m_localVertices[indices->count * 4 + j].position.Set(glyph.corners[j]);
				m_localVertices[indices->count * 4 + j].uv.Set(uvRect.GetCorner((glyph.flipped) ? flippedCorners[j] : normalCorners[j]));
			}

			// Increment the counter, go to next glyph
			indices->count++;
		}

		m_localBounds = drawer.GetBounds();

		InvalidateBoundingVolume();
		InvalidateInstanceData(0);

		clearOnFail.Reset();
	}

	/*
	* \brief Makes the bounding volume of this text
	*/

	void TextSprite::MakeBoundingVolume() const
	{
		Rectf bounds(m_localBounds);
		Vector2f max = m_scale * bounds.GetMaximum();
		Vector2f min = m_scale * bounds.GetMinimum();

		m_boundingVolume.Set(min.x * Vector3f::Right() + min.y * Vector3f::Down(), max.x * Vector3f::Right() + max.y * Vector3f::Down());
	}

	/*!
	* \brief Handle the invalidation of an atlas
	*
	* \param atlas Atlas being invalidated
	*/

	void TextSprite::OnAtlasInvalidated(const AbstractAtlas* atlas)
	{
		#ifdef NAZARA_DEBUG
		if (m_atlases.find(atlas) == m_atlases.end())
		{
			NazaraInternalError("Not listening to " + String::Pointer(atlas));
			return;
		}
		#endif

		NazaraWarning("TextSprite " + String::Pointer(this) + " has been cleared because atlas " + String::Pointer(atlas) + " has been invalidated (cleared or released)");
		Clear();
	}

	/*!
	* \brief Handle the invalidation of an atlas layer
	*
	* \param atlas Atlas being invalidated
	* \param oldLayer Pointer to the previous layer
	* \param newLayer Pointer to the new layer
	*/

	void TextSprite::OnAtlasLayerChange(const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer)
	{
		NazaraUnused(atlas);

		#ifdef NAZARA_DEBUG
		if (m_atlases.find(atlas) == m_atlases.end())
		{
			NazaraInternalError("Not listening to " + String::Pointer(atlas));
			return;
		}
		#endif

		// The texture of an atlas have just been recreated (size change)
		// we have to adjust the coordinates of the texture and the rendering texture
		Texture* oldTexture = static_cast<Texture*>(oldLayer);
		Texture* newTexture = static_cast<Texture*>(newLayer);

		// It is possible that we don't use the texture (the atlas warning us for each of its layers)
		auto it = m_renderInfos.find(oldTexture);
		if (it != m_renderInfos.end())
		{
			// We indeed use this texture, we have to update its coordinates
			RenderIndices indices = std::move(it->second);

			Vector2ui oldSize(oldTexture->GetSize());
			Vector2ui newSize(newTexture->GetSize());
			Vector2f scale = Vector2f(oldSize) / Vector2f(newSize); // ratio of the old one to the new one

			// Now we will iterate through each coordinates of the concerned texture to multiply them by the ratio
			SparsePtr<Vector2f> texCoordPtr(&m_localVertices[indices.first].uv, sizeof(VertexStruct_XYZ_Color_UV));
			for (unsigned int i = 0; i < indices.count; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
					m_localVertices[i*4 + j].uv *= scale;
			}

			// We get rid off the old texture and we set the new one at the place (same for indices)
			m_renderInfos.erase(it);
			m_renderInfos.insert(std::make_pair(newTexture, std::move(indices)));
		}
	}

	/*!
	* \brief Updates the data of the sprite
	*
	* \param instanceData Data of the instance
	*/

	void TextSprite::UpdateData(InstanceData* instanceData) const
	{
		instanceData->data.resize(m_localVertices.size() * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		SparsePtr<Color> colorPtr(&vertices[0].color, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector3f> posPtr(&vertices[0].position, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector2f> texCoordPtr(&vertices[0].uv, sizeof(VertexStruct_XYZ_Color_UV));

		// We will not initialize the final vertices (those send to the RenderQueue)
		// With the help of the coordinates axis, the matrix and our color attribute
		for (auto& pair : m_renderInfos)
		{
			RenderIndices& indices = pair.second;
			if (indices.count == 0)
				continue; //< Ignore empty render indices

			SparsePtr<Color> color = colorPtr + indices.first * 4;
			SparsePtr<Vector3f> pos = posPtr + indices.first * 4;
			SparsePtr<Vector2f> uv = texCoordPtr + indices.first * 4;
			VertexStruct_XY_Color_UV* localVertex = &m_localVertices[indices.first * 4];
			for (unsigned int i = 0; i < indices.count; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
				{
					Vector3f localPos = localVertex->position.x*Vector3f::Right() + localVertex->position.y*Vector3f::Down();
					localPos *= m_scale;

					*pos++ = instanceData->transformMatrix.Transform(localPos);
					*color++ = m_color * localVertex->color;
					*uv++ = localVertex->uv;

					localVertex++;
				}
			}
		}
	}

	TextSpriteLibrary::LibraryMap TextSprite::s_library;
}
