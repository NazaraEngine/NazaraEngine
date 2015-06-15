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

NzTextSprite::NzTextSprite() :
m_color(NzColor::White),
m_scale(1.f)
{
	SetDefaultMaterial();
}

NzTextSprite::NzTextSprite(const NzTextSprite& sprite) :
NzInstancedRenderable(sprite),
m_renderInfos(sprite.m_renderInfos),
m_localVertices(sprite.m_localVertices),
m_color(sprite.m_color),
m_material(sprite.m_material),
m_localBounds(sprite.m_localBounds),
m_scale(sprite.m_scale)
{
	for (auto it = sprite.m_atlases.begin(); it != sprite.m_atlases.end(); ++it)
	{
		const NzAbstractAtlas* atlas = it->first;
		AtlasSlots& slots = m_atlases[atlas];

		slots.clearSlot.Connect(atlas->OnAtlasCleared, this, &NzTextSprite::OnAtlasInvalidated);
		slots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &NzTextSprite::OnAtlasLayerChange);
		slots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &NzTextSprite::OnAtlasInvalidated);
	}
}

void NzTextSprite::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
{
	for (auto& pair : m_renderInfos)
	{
		NzTexture* overlay = pair.first;
		RenderIndices& indices = pair.second;

		if (indices.count > 0)
		{
			const NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const NzVertexStruct_XYZ_Color_UV*>(instanceData.data.data());
			renderQueue->AddSprites(m_material, &vertices[indices.first*4], indices.count, overlay);
		}
	}
}

void NzTextSprite::Clear()
{
	m_atlases.clear();
	m_boundingVolume.MakeNull();
	m_localVertices.clear();
	m_renderInfos.clear();
}

NzTextSprite* NzTextSprite::Clone() const
{
	return new NzTextSprite(*this);
}

NzTextSprite* NzTextSprite::Create() const
{
	return new NzTextSprite;
}

const NzColor& NzTextSprite::GetColor() const
{
	return m_color;
}

NzMaterial* NzTextSprite::GetMaterial() const
{
	return m_material;
}

float NzTextSprite::GetScale() const
{
	return m_scale;
}

void NzTextSprite::InvalidateVertices()
{
	InvalidateInstanceData(0);
}

bool NzTextSprite::IsDrawable() const
{
	return m_material != nullptr;
}

void NzTextSprite::SetColor(const NzColor& color)
{
	m_color = color;

	InvalidateVertices();
}

void NzTextSprite::SetDefaultMaterial()
{
	NzMaterialRef material = NzMaterial::New();
	material->Enable(nzRendererParameter_Blend, true);
	material->Enable(nzRendererParameter_DepthWrite, false);
	material->Enable(nzRendererParameter_FaceCulling, false);
	material->EnableLighting(false);
	material->SetDstBlend(nzBlendFunc_InvSrcAlpha);
	material->SetSrcBlend(nzBlendFunc_SrcAlpha);

	SetMaterial(material);
}

void NzTextSprite::SetMaterial(NzMaterial* material)
{
	m_material = material;
}

void NzTextSprite::SetScale(float scale)
{
	m_scale = scale;

	InvalidateInstanceData(0);
}

void NzTextSprite::Update(const NzAbstractTextDrawer& drawer)
{
	m_atlases.clear();

	NzCallOnExit clearOnFail([this]()
	{
		Clear();
	});

	unsigned int fontCount = drawer.GetFontCount();
	for (unsigned int i = 0; i < fontCount; ++i)
	{
		NzFont* font = drawer.GetFont(i);
		const NzAbstractAtlas* atlas = font->GetAtlas().get();

		#if NAZARA_GRAPHICS_SAFE
		if ((atlas->GetStorage() & nzDataStorage_Hardware) == 0)
		{
			// Cet atlas ne nous donnera pas de texture, nous ne pouvons pas l'utiliser
			NazaraError("Font " + NzString::Pointer(font) + " uses a non-hardware atlas which cannot be used by text sprites");
			return;
		}
		#endif

		if (m_atlases.find(atlas) == m_atlases.end())
		{
			AtlasSlots& slots = m_atlases[atlas];

			slots.clearSlot.Connect(atlas->OnAtlasCleared, this, &NzTextSprite::OnAtlasInvalidated);
			slots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &NzTextSprite::OnAtlasLayerChange);
			slots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &NzTextSprite::OnAtlasInvalidated);
		}
	}

	unsigned int glyphCount = drawer.GetGlyphCount();
	m_localVertices.resize(glyphCount * 4);

	NzTexture* lastTexture = nullptr;
	unsigned int* count = nullptr;
	for (unsigned int i = 0; i < glyphCount; ++i)
	{
		const NzAbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);

		NzTexture* texture = static_cast<NzTexture*>(glyph.atlas);
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
		const NzAbstractTextDrawer::Glyph& glyph = drawer.GetGlyph(i);

		NzTexture* texture = static_cast<NzTexture*>(glyph.atlas);
		if (lastTexture != texture)
		{
			indices = &m_renderInfos[texture]; // On a changé de texture, on ajuste le pointeur
			lastTexture = texture;
		}

		// On commence par transformer les coordonnées entières en flottantes:
		NzVector2ui size(texture->GetSize());
		float invWidth = 1.f/size.x;
		float invHeight = 1.f/size.y;

		NzRectf uvRect(glyph.atlasRect);
		uvRect.x *= invWidth;
		uvRect.y *= invHeight;
		uvRect.width *= invWidth;
		uvRect.height *= invHeight;

		static nzRectCorner normalCorners[4] = {nzRectCorner_LeftTop, nzRectCorner_RightTop, nzRectCorner_LeftBottom, nzRectCorner_RightBottom};
		static nzRectCorner flippedCorners[4] = {nzRectCorner_LeftBottom, nzRectCorner_LeftTop, nzRectCorner_RightBottom, nzRectCorner_RightTop};

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

NzTextSprite& NzTextSprite::operator=(const NzTextSprite& text)
{
	NzInstancedRenderable::operator=(text);

	m_atlases.clear();

	m_color = text.m_color;
	m_material = text.m_material;
	m_renderInfos = text.m_renderInfos;
	m_localBounds = text.m_localBounds;
	m_localVertices = text.m_localVertices;
	m_scale = text.m_scale;

	// Connect to the slots of the new atlases
	for (auto it = text.m_atlases.begin(); it != text.m_atlases.end(); ++it)
	{
		const NzAbstractAtlas* atlas = it->first;
		AtlasSlots& slots = m_atlases[atlas];

		slots.clearSlot.Connect(atlas->OnAtlasCleared, this, &NzTextSprite::OnAtlasInvalidated);
		slots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &NzTextSprite::OnAtlasLayerChange);
		slots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &NzTextSprite::OnAtlasInvalidated);
	}

	InvalidateBoundingVolume();
	InvalidateVertices();

	return *this;
}

void NzTextSprite::MakeBoundingVolume() const
{
	NzRectf bounds(m_localBounds);
	NzVector2f max = bounds.GetMaximum();
	NzVector2f min = bounds.GetMinimum();

	m_boundingVolume.Set(min.x*NzVector3f::Right() + min.y*NzVector3f::Down(), max.x*NzVector3f::Right() + max.y*NzVector3f::Down());
}

void NzTextSprite::OnAtlasInvalidated(const NzAbstractAtlas* atlas)
{
	#ifdef NAZARA_DEBUG
	if (m_atlases.find(atlas) == m_atlases.end())
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(atlas));
		return;
	}
	#endif

	NazaraWarning("TextSprite " + NzString::Pointer(this) + " has been cleared because atlas " + NzString::Pointer(atlas) + " has been invalidated (cleared or released)");
	Clear();
}

void NzTextSprite::OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer)
{
	NazaraUnused(atlas);

	#ifdef NAZARA_DEBUG
	if (m_atlases.find(atlas) == m_atlases.end())
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(atlas));
		return;
	}
	#endif

	// La texture d'un atlas vient d'être recréée (changement de taille)
	// nous devons ajuster les coordonnées de textures et la texture du rendu
	NzTexture* oldTexture = static_cast<NzTexture*>(oldLayer);
	NzTexture* newTexture = static_cast<NzTexture*>(newLayer);

	// Il est possible que nous n'utilisions pas la texture en question (l'atlas nous prévenant pour chacun de ses layers)
	auto it = m_renderInfos.find(oldTexture);
	if (it != m_renderInfos.end())
	{
		// Nous utilisons bien cette texture, nous devons mettre à jour les coordonnées de texture
		RenderIndices indices = std::move(it->second);

		NzVector2ui oldSize(oldTexture->GetSize());
		NzVector2ui newSize(newTexture->GetSize());
		NzVector2f scale = NzVector2f(oldSize)/NzVector2f(newSize); // ratio ancienne et nouvelle taille

		// On va maintenant parcourir toutes les coordonnées de texture concernées pour les multiplier par ce ratio
		NzSparsePtr<NzVector2f> texCoordPtr(&m_localVertices[indices.first].uv, sizeof(NzVertexStruct_XYZ_Color_UV));
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

void NzTextSprite::UpdateData(InstanceData* instanceData) const
{
	instanceData->data.resize(m_localVertices.size() * sizeof(NzVertexStruct_XYZ_Color_UV));
	NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_Color_UV*>(instanceData->data.data());

	NzSparsePtr<NzColor> colorPtr(&vertices[0].color, sizeof(NzVertexStruct_XYZ_Color_UV));
	NzSparsePtr<NzVector3f> posPtr(&vertices[0].position, sizeof(NzVertexStruct_XYZ_Color_UV));
	NzSparsePtr<NzVector2f> texCoordPtr(&vertices[0].uv, sizeof(NzVertexStruct_XYZ_Color_UV));

	// Nous allons maintenant initialiser les sommets finaux (ceux envoyés à la RenderQueue)
	// à l'aide du repère, de la matrice et de notre attribut de couleur
	for (auto& pair : m_renderInfos)
	{
		RenderIndices& indices = pair.second;

		NzSparsePtr<NzColor> color = colorPtr + indices.first*4;
		NzSparsePtr<NzVector3f> pos = posPtr + indices.first*4;
		NzSparsePtr<NzVector2f> uv = texCoordPtr + indices.first*4;
		NzVertexStruct_XY_Color_UV* localVertex = &m_localVertices[indices.first*4];
		for (unsigned int i = 0; i < indices.count; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				NzVector3f localPos = localVertex->position.x*NzVector3f::Right() + localVertex->position.y*NzVector3f::Down();
				localPos *= m_scale;

				*pos++ = instanceData->transformMatrix.Transform(localPos);
				*color++ = m_color * localVertex->color;
				*uv++ = localVertex->uv;

				localVertex++;
			}
		}
	}
}

NzTextSpriteLibrary::LibraryMap NzTextSprite::s_library;
