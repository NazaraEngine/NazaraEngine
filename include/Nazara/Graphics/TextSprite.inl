// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

inline NzTextSprite::NzTextSprite() :
m_color(NzColor::White),
m_scale(1.f)
{
	SetDefaultMaterial();
}

inline NzTextSprite::NzTextSprite(const NzTextSprite& sprite) :
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

inline void NzTextSprite::Clear()
{
	m_atlases.clear();
	m_boundingVolume.MakeNull();
	m_localVertices.clear();
	m_renderInfos.clear();
}

inline const NzColor& NzTextSprite::GetColor() const
{
	return m_color;
}

inline const NzMaterialRef& NzTextSprite::GetMaterial() const
{
	return m_material;
}

inline float NzTextSprite::GetScale() const
{
	return m_scale;
}

inline void NzTextSprite::SetColor(const NzColor& color)
{
	m_color = color;

	InvalidateVertices();
}

inline void NzTextSprite::SetDefaultMaterial()
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

inline void NzTextSprite::SetMaterial(NzMaterialRef material)
{
	m_material = std::move(material);
}

inline void NzTextSprite::SetScale(float scale)
{
	m_scale = scale;

	InvalidateVertices();
}

inline void NzTextSprite::InvalidateVertices()
{
	InvalidateInstanceData(0);
}

inline NzTextSprite& NzTextSprite::operator=(const NzTextSprite& text)
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

template<typename... Args>
NzTextSpriteRef NzTextSprite::New(Args&&... args)
{
	std::unique_ptr<NzTextSprite> object(new NzTextSprite(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Renderer/DebugOff.hpp>
