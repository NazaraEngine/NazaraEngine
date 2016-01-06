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
				renderQueue->AddSprites(instanceData.renderOrder, m_material, &vertices[indices.first*4], indices.count, overlay);
			}
		}
	}

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
		unsigned int fontCount = drawer.GetFontCount();
		for (unsigned int i = 0; i < fontCount; ++i)
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
		auto it = m_atlases.begin();
		while (it != m_atlases.end())
		{
			if (!it->second.used)
				m_atlases.erase(it++);
			else
				++it;
		}

		unsigned int glyphCount = drawer.GetGlyphCount();
		m_localVertices.resize(glyphCount * 4);

		Texture* lastTexture = nullptr;
		unsigned int* count = nullptr;
		for (unsigned int i = 0; i < glyphCount; ++i)
		{
			const AbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);

			Texture* texture = static_cast<Texture*>(glyph.atlas);
			if (lastTexture != texture)
			{
				auto pair = m_renderInfos.insert(std::make_pair(texture, RenderIndices{0U, 0U}));

				count = &pair.first->second.count;
				lastTexture = texture;
			}

			(*count)++;
		}

		// Attribution des indices
		unsigned int index = 0;
		for (auto& pair : m_renderInfos)
		{
			RenderIndices& indices = pair.second;

			indices.first = index;

			index += indices.count;
			indices.count = 0; // On réinitialise count à zéro (on va s'en servir comme compteur dans la boucle suivante)
		}

		lastTexture = nullptr;
		RenderIndices* indices = nullptr;
		for (unsigned int i = 0; i < glyphCount; ++i)
		{
			const AbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);

			Texture* texture = static_cast<Texture*>(glyph.atlas);
			if (lastTexture != texture)
			{
				indices = &m_renderInfos[texture]; // On a changé de texture, on ajuste le pointeur
				lastTexture = texture;
			}

			// On commence par transformer les coordonnées entières en flottantes:
			Vector2ui size(texture->GetSize());
			float invWidth = 1.f/size.x;
			float invHeight = 1.f/size.y;

			Rectf uvRect(glyph.atlasRect);
			uvRect.x *= invWidth;
			uvRect.y *= invHeight;
			uvRect.width *= invWidth;
			uvRect.height *= invHeight;

			static RectCorner normalCorners[4] = {RectCorner_LeftTop, RectCorner_RightTop, RectCorner_LeftBottom, RectCorner_RightBottom};
			static RectCorner flippedCorners[4] = {RectCorner_LeftBottom, RectCorner_LeftTop, RectCorner_RightBottom, RectCorner_RightTop};

			// Affectation des positions, couleurs, coordonnées de textures
			for (unsigned int j = 0; j < 4; ++j)
			{
				// Remember that indices->count is a counter here, not a count value
				m_localVertices[indices->count*4 + j].color = glyph.color;
				m_localVertices[indices->count*4 + j].position.Set(glyph.corners[j]);
				m_localVertices[indices->count*4 + j].uv.Set(uvRect.GetCorner((glyph.flipped) ? flippedCorners[j] : normalCorners[j]));
			}

			// Et on passe au prochain sommet
			indices->count++;
		}

		m_localBounds = drawer.GetBounds();

		InvalidateBoundingVolume();
		InvalidateInstanceData(0);

		clearOnFail.Reset();
	}

	void TextSprite::MakeBoundingVolume() const
	{
		Rectf bounds(m_localBounds);
		Vector2f max = bounds.GetMaximum();
		Vector2f min = bounds.GetMinimum();

		m_boundingVolume.Set(min.x*Vector3f::Right() + min.y*Vector3f::Down(), max.x*Vector3f::Right() + max.y*Vector3f::Down());
	}

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

		// La texture d'un atlas vient d'être recréée (changement de taille)
		// nous devons ajuster les coordonnées de textures et la texture du rendu
		Texture* oldTexture = static_cast<Texture*>(oldLayer);
		Texture* newTexture = static_cast<Texture*>(newLayer);

		// Il est possible que nous n'utilisions pas la texture en question (l'atlas nous prévenant pour chacun de ses layers)
		auto it = m_renderInfos.find(oldTexture);
		if (it != m_renderInfos.end())
		{
			// Nous utilisons bien cette texture, nous devons mettre à jour les coordonnées de texture
			RenderIndices indices = std::move(it->second);

			Vector2ui oldSize(oldTexture->GetSize());
			Vector2ui newSize(newTexture->GetSize());
			Vector2f scale = Vector2f(oldSize)/Vector2f(newSize); // ratio ancienne et nouvelle taille

			// On va maintenant parcourir toutes les coordonnées de texture concernées pour les multiplier par ce ratio
			SparsePtr<Vector2f> texCoordPtr(&m_localVertices[indices.first].uv, sizeof(VertexStruct_XYZ_Color_UV));
			for (unsigned int i = 0; i < indices.count; ++i)
			{
				for (unsigned int j = 0; j < 4; ++j)
					m_localVertices[i*4 + j].uv *= scale;
			}

			// Nous enlevons l'ancienne texture et rajoutons la nouvelle à sa place (pour les mêmes indices)
			m_renderInfos.erase(it);
			m_renderInfos.insert(std::make_pair(newTexture, std::move(indices)));
		}
	}

	void TextSprite::UpdateData(InstanceData* instanceData) const
	{
		instanceData->data.resize(m_localVertices.size() * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		SparsePtr<Color> colorPtr(&vertices[0].color, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector3f> posPtr(&vertices[0].position, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector2f> texCoordPtr(&vertices[0].uv, sizeof(VertexStruct_XYZ_Color_UV));

		// Nous allons maintenant initialiser les sommets finaux (ceux envoyés à la RenderQueue)
		// à l'aide du repère, de la matrice et de notre attribut de couleur
		for (auto& pair : m_renderInfos)
		{
			RenderIndices& indices = pair.second;
			if (indices.count == 0)
				continue; //< Ignore empty render indices

			SparsePtr<Color> color = colorPtr + indices.first*4;
			SparsePtr<Vector3f> pos = posPtr + indices.first*4;
			SparsePtr<Vector2f> uv = texCoordPtr + indices.first*4;
			VertexStruct_XY_Color_UV* localVertex = &m_localVertices[indices.first*4];
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
