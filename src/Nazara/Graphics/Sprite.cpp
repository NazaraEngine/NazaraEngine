// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzSprite::NzSprite() :
m_boundingVolume(NzBoundingVolumef::Null()),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f),
m_boundingVolumeUpdated(true)
{
}

NzSprite::NzSprite(const NzSprite& sprite) :
NzSceneNode(sprite),
m_boundingVolume(sprite.m_boundingVolume),
m_material(sprite.m_material),
m_textureCoords(sprite.m_textureCoords),
m_size(sprite.m_size),
m_boundingVolumeUpdated(sprite.m_boundingVolumeUpdated)
{
}

NzSprite::NzSprite(NzSprite&& sprite) :
NzSceneNode(sprite),
m_boundingVolume(sprite.m_boundingVolume),
m_material(std::move(sprite.m_material)),
m_textureCoords(sprite.m_textureCoords),
m_size(sprite.m_size),
m_boundingVolumeUpdated(sprite.m_boundingVolumeUpdated)
{
}

NzSprite::~NzSprite() = default;

void NzSprite::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	renderQueue->AddSprite(this);
}

const NzBoundingVolumef& NzSprite::GetBoundingVolume() const
{
	static NzBoundingVolumef infinity(NzBoundingVolumef::Infinite());
	return infinity;
}

NzMaterial* NzSprite::GetMaterial() const
{
	return m_material;
}

nzSceneNodeType NzSprite::GetSceneNodeType() const
{
	return nzSceneNodeType_Sprite;
}

const NzVector2f& NzSprite::GetSize() const
{
	return m_size;
}

const NzRectf& NzSprite::GetTextureCoords() const
{
	return m_textureCoords;
}

void NzSprite::SetMaterial(NzMaterial* material)
{
	m_material = material;
}

void NzSprite::SetSize(const NzVector2f& size)
{
	m_size = size;
	m_boundingVolume.MakeNull();
}

void NzSprite::SetTextureCoords(const NzRectf& coords)
{
	m_textureCoords = coords;
}

void NzSprite::SetTextureRect(const NzRectui& rect)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_material)
	{
		NazaraError("Sprite has no material");
		return;
	}

	if (!m_material->HasDiffuseMap())
	{
		NazaraError("Sprite material has no diffuse map");
		return;
	}
	#endif

	NzTexture* diffuseMap = m_material->GetDiffuseMap();

	float invWidth = 1.f/diffuseMap->GetWidth();
	float invHeight = 1.f/diffuseMap->GetHeight();

	SetTextureCoords(NzRectf(invWidth*rect.x, invHeight*rect.y, invWidth*rect.width, invHeight*rect.height));
}

bool NzSprite::FrustumCull(const NzFrustumf& frustum)
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return frustum.Contains(m_boundingVolume);
}

void NzSprite::Invalidate()
{
	NzSceneNode::Invalidate();

	m_boundingVolumeUpdated = false;
}

void NzSprite::Register()
{
}

void NzSprite::Unregister()
{
}

void NzSprite::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
		m_boundingVolume.Set(-m_size.x*0.5f, -m_size.y*0.5f, 0, m_size.x, m_size.y, 1.f);

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingVolume.Update(m_transformMatrix);
	m_boundingVolumeUpdated = true;
}
