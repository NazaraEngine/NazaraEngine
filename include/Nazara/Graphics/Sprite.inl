// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

inline NzSprite::NzSprite() :
m_color(NzColor::White),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f)
{
	SetDefaultMaterial();
}

inline NzSprite::NzSprite(NzMaterialRef material) :
m_color(NzColor::White),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f)
{
	SetMaterial(std::move(material), true);
}

inline NzSprite::NzSprite(NzTexture* texture) :
m_color(NzColor::White),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f)
{
	SetTexture(texture, true);
}

inline NzSprite::NzSprite(const NzSprite& sprite) :
NzInstancedRenderable(sprite),
m_color(sprite.m_color),
m_material(sprite.m_material),
m_textureCoords(sprite.m_textureCoords),
m_size(sprite.m_size)
{
}

inline const NzColor& NzSprite::GetColor() const
{
	return m_color;
}

inline const NzMaterialRef& NzSprite::GetMaterial() const
{
	return m_material;
}

inline const NzVector2f& NzSprite::GetSize() const
{
	return m_size;
}

inline const NzRectf& NzSprite::GetTextureCoords() const
{
	return m_textureCoords;
}

inline void NzSprite::SetColor(const NzColor& color)
{
	m_color = color;

	InvalidateVertices();
}

inline void NzSprite::SetDefaultMaterial()
{
	NzMaterialRef material = NzMaterial::New();
	material->Enable(nzRendererParameter_FaceCulling, false);
	material->EnableLighting(false);

	SetMaterial(material);
}

inline void NzSprite::SetMaterial(NzMaterialRef material, bool resizeSprite)
{
	m_material = std::move(material);
	if (m_material && resizeSprite)
	{
		NzTexture* diffuseMap = m_material->GetDiffuseMap();
		if (diffuseMap && diffuseMap->IsValid())
			SetSize(NzVector2f(NzVector2ui(diffuseMap->GetSize())));
	}
}

inline void NzSprite::SetSize(const NzVector2f& size)
{
	m_size = size;

	// On invalide la bounding box
	InvalidateBoundingVolume();
	InvalidateVertices();
}

inline void NzSprite::SetSize(float sizeX, float sizeY)
{
	SetSize(NzVector2f(sizeX, sizeY));
}

inline void NzSprite::SetTexture(NzTextureRef texture, bool resizeSprite)
{
	if (!m_material)
		SetDefaultMaterial();
	else if (m_material->GetReferenceCount() > 1)
		m_material = NzMaterial::New(*m_material); // Copie

	if (resizeSprite && texture && texture->IsValid())
		SetSize(NzVector2f(NzVector2ui(texture->GetSize())));

	m_material->SetDiffuseMap(std::move(texture));
}

inline void NzSprite::SetTextureCoords(const NzRectf& coords)
{
	m_textureCoords = coords;
	InvalidateVertices();
}

inline void NzSprite::SetTextureRect(const NzRectui& rect)
{
	NazaraAssert(m_material, "Sprite has no material");
	NazaraAssert(m_material->HasDiffuseMap(), "Sprite material has no diffuse map");

	NzTexture* diffuseMap = m_material->GetDiffuseMap();

	float invWidth = 1.f/diffuseMap->GetWidth();
	float invHeight = 1.f/diffuseMap->GetHeight();

	SetTextureCoords(NzRectf(invWidth*rect.x, invHeight*rect.y, invWidth*rect.width, invHeight*rect.height));
}

inline NzSprite& NzSprite::operator=(const NzSprite& sprite)
{
	NzInstancedRenderable::operator=(sprite);

	m_color = sprite.m_color;
	m_material = sprite.m_material;
	m_textureCoords = sprite.m_textureCoords;
	m_size = sprite.m_size;

	// On ne copie pas les sommets finaux car il est très probable que nos paramètres soient modifiés et qu'ils doivent être régénérés de toute façon
	InvalidateBoundingVolume();
	InvalidateVertices();

	return *this;
}

inline void NzSprite::InvalidateVertices()
{
	InvalidateInstanceData(0);
}

template<typename... Args>
NzSpriteRef NzSprite::New(Args&&... args)
{
	std::unique_ptr<NzSprite> object(new NzSprite(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Renderer/DebugOff.hpp>
