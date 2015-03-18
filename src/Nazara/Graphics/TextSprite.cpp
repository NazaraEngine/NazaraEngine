// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <memory>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzTextSprite::NzTextSprite() :
m_color(NzColor::White),
m_verticesUpdated(false)
{
	SetDefaultMaterial();
}

NzTextSprite::NzTextSprite(const NzTextSprite& sprite) :
NzSceneNode(sprite),
m_atlases(sprite.m_atlases),
m_renderInfos(sprite.m_renderInfos),
m_localVertices(sprite.m_localVertices),
m_vertices(sprite.m_vertices),
m_color(sprite.m_color),
m_material(sprite.m_material),
m_localBounds(sprite.m_localBounds),
m_verticesUpdated(sprite.m_verticesUpdated)
{
	SetParent(sprite.GetParent());

	for (const NzAbstractAtlas* atlas : m_atlases)
		atlas->AddListener(this);
}

NzTextSprite::~NzTextSprite()
{
	ClearAtlases();
}

void NzTextSprite::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	if (!m_verticesUpdated)
		UpdateVertices();

	for (auto& pair : m_renderInfos)
	{
		NzTexture* overlay = pair.first;
		RenderIndices& indices = pair.second;

		if (indices.count > 0)
			renderQueue->AddSprites(m_material, &m_vertices[indices.first*4], indices.count, overlay);
	}
}

void NzTextSprite::Clear()
{
	ClearAtlases();
	m_boundingVolume.MakeNull();
	m_localVertices.clear();
	m_renderInfos.clear();
	m_vertices.clear();
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

nzSceneNodeType NzTextSprite::GetSceneNodeType() const
{
	return nzSceneNodeType_TextSprite;
}

void NzTextSprite::InvalidateVertices()
{
	m_verticesUpdated = false;
}

bool NzTextSprite::IsDrawable() const
{
	return m_material != nullptr;
}

void NzTextSprite::SetColor(const NzColor& color)
{
	m_color = color;
	m_verticesUpdated = false;
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

void NzTextSprite::Update(const NzAbstractTextDrawer& drawer)
{
	ClearAtlases();

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

		if (m_atlases.insert(atlas).second)
			atlas->AddListener(this);
	}

	unsigned int glyphCount = drawer.GetGlyphCount();
	m_localVertices.resize(glyphCount * 4);
	m_vertices.resize(glyphCount * 4);

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

	NzSparsePtr<NzVector2f> texCoordPtr(&m_vertices[0].uv, sizeof(NzVertexStruct_XYZ_Color_UV));

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

		// Affectation des positions et couleurs (locaux)
		for (unsigned int j = 0; j < 4; ++j)
		{
			m_localVertices[i*4 + j].color = glyph.color;
			m_localVertices[i*4 + j].position.Set(glyph.corners[j]);
		}

		// Calcul des coordonnées de texture (globaux)

		// On commence par transformer les coordonnées entières en flottantes:
		NzVector2ui size(texture->GetSize());
		float invWidth = 1.f/size.x;
		float invHeight = 1.f/size.y;

		NzRectf uvRect(glyph.atlasRect);
		uvRect.x *= invWidth;
		uvRect.y *= invHeight;
		uvRect.width *= invWidth;
		uvRect.height *= invHeight;

		// Extraction des quatre coins et attribution
		NzSparsePtr<NzVector2f> texCoord = texCoordPtr + indices->first*4 + indices->count*4;
		if (!glyph.flipped)
		{
			// Le glyphe n'est pas retourné, l'ordre des UV suit celui des sommets
			*texCoord++ = uvRect.GetCorner(nzRectCorner_LeftTop);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_RightTop);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_LeftBottom);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_RightBottom);
		}
		else
		{
			// Le glyphe a subit une rotation de 90° (sens antihoraire), on adapte les UV en conséquence
			*texCoord++ = uvRect.GetCorner(nzRectCorner_LeftBottom);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_LeftTop);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_RightBottom);
			*texCoord++ = uvRect.GetCorner(nzRectCorner_RightTop);
		}

		// Et on passe au prochain sommet
		indices->count++;
	}

	m_localBounds = drawer.GetBounds();
	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
	m_verticesUpdated = false;

	clearOnFail.Reset();
}

NzTextSprite& NzTextSprite::operator=(const NzTextSprite& text)
{
	NzSceneNode::operator=(text);

	m_atlases = text.m_atlases;
	m_color = text.m_color;
	m_material = text.m_material;
	m_renderInfos = text.m_renderInfos;
	m_localBounds = text.m_localBounds;
	m_localVertices = text.m_localVertices;

	// On ne copie pas les sommets finaux car il est très probable que nos paramètres soient modifiés et qu'ils doivent être régénérés de toute façon
	m_verticesUpdated = false;

	return *this;
}

void NzTextSprite::ClearAtlases()
{
	for (const NzAbstractAtlas* atlas : m_atlases)
		atlas->RemoveListener(this);

	m_atlases.clear();
}

void NzTextSprite::InvalidateNode()
{
	NzSceneNode::InvalidateNode();

	m_verticesUpdated = false;
}

void NzTextSprite::MakeBoundingVolume() const
{
	NzVector3f down = (m_scene) ? m_scene->GetDown() : NzVector3f::Down();
	NzVector3f right = (m_scene) ? m_scene->GetRight() : NzVector3f::Right();

	NzRectf bounds(m_localBounds);
	NzVector2f max = bounds.GetMaximum();
	NzVector2f min = bounds.GetMinimum();

	m_boundingVolume.Set(min.x*right + min.y*down, max.x*right + max.y*down);
}

bool NzTextSprite::OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(userdata);

	#ifdef NAZARA_DEBUG
	if (m_atlases.find(atlas) == m_atlases.end())
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(atlas));
		return false;
	}
	#endif

	NazaraWarning("TextSprite " + NzString::Pointer(this) + " has been cleared because atlas " + NzString::Pointer(atlas) + " that was under use has been cleared");
	Clear();

	return false;
}

bool NzTextSprite::OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

	#ifdef NAZARA_DEBUG
	if (m_atlases.find(atlas) == m_atlases.end())
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(atlas));
		return false;
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
		NzSparsePtr<NzVector2f> texCoordPtr(&m_vertices[indices.first].uv, sizeof(NzVertexStruct_XYZ_Color_UV));
		for (unsigned int i = 0; i < indices.count; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
				texCoordPtr[i*4 + j] *= scale;
		}

		// Nous enlevons l'ancienne texture et rajoutons la nouvelle à sa place (pour les mêmes indices)
		m_renderInfos.erase(it);
		m_renderInfos.insert(std::make_pair(newTexture, std::move(indices)));
	}

	return true;
}

void NzTextSprite::OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(userdata);

	#ifdef NAZARA_DEBUG
	if (m_atlases.find(atlas) == m_atlases.end())
	{
		NazaraInternalError("Not listening to " + NzString::Pointer(atlas));
		return;
	}
	#endif

	// L'atlas a été libéré alors que le TextSprite l'utilisait encore, notre seule option (pour éviter un crash) est de nous réinitialiser
	NazaraWarning("TextSprite " + NzString::Pointer(this) + " has been cleared because atlas " + NzString::Pointer(atlas) + " has been released");
	Clear();
}

void NzTextSprite::Register()
{
	// Le changement de scène peut affecter les sommets
	m_verticesUpdated = false;
}

void NzTextSprite::Unregister()
{
}

void NzTextSprite::UpdateVertices() const
{
	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	// On récupère le repère de la scène
	NzVector3f down = (m_scene) ? m_scene->GetDown() : NzVector3f::Down();
	NzVector3f right = (m_scene) ? m_scene->GetRight() : NzVector3f::Right();

	NzSparsePtr<NzColor> colorPtr(&m_vertices[0].color, sizeof(NzVertexStruct_XYZ_Color_UV));
	NzSparsePtr<NzVector3f> posPtr(&m_vertices[0].position, sizeof(NzVertexStruct_XYZ_Color_UV));

	// Nous allons maintenant initialiser les sommets finaux (ceux envoyés à la RenderQueue)
	// à l'aide du repère, de la matrice et de notre attribut de couleur
	for (auto& pair : m_renderInfos)
	{
		RenderIndices& indices = pair.second;

		NzSparsePtr<NzColor> color = colorPtr + indices.first*4;
		NzSparsePtr<NzVector3f> pos = posPtr + indices.first*4;
		NzVertexStruct_XY_Color* localVertex = &m_localVertices[indices.first*4];
		for (unsigned int i = 0; i < indices.count; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				*pos++ = m_transformMatrix.Transform(localVertex->position.x*right + localVertex->position.y*down);
				*color++ = m_color * localVertex->color;

				localVertex++;
			}
		}
	}

	m_verticesUpdated = true;
}
