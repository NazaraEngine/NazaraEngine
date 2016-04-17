// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline TextSprite::TextSprite() :
	m_color(Color::White),
	m_scale(1.f)
	{
		SetDefaultMaterial();
	}

	inline TextSprite::TextSprite(const AbstractTextDrawer& drawer) :
	TextSprite()
	{
		Update(drawer);
	}

	inline TextSprite::TextSprite(const TextSprite& sprite) :
	InstancedRenderable(sprite),
	m_renderInfos(sprite.m_renderInfos),
	m_localVertices(sprite.m_localVertices),
	m_color(sprite.m_color),
	m_material(sprite.m_material),
	m_localBounds(sprite.m_localBounds),
	m_scale(sprite.m_scale)
	{
		for (auto it = sprite.m_atlases.begin(); it != sprite.m_atlases.end(); ++it)
		{
			const AbstractAtlas* atlas = it->first;
			AtlasSlots& atlasSlots = m_atlases[atlas];

			atlasSlots.clearSlot.Connect(atlas->OnAtlasCleared, this, &TextSprite::OnAtlasInvalidated);
			atlasSlots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &TextSprite::OnAtlasLayerChange);
			atlasSlots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &TextSprite::OnAtlasInvalidated);
		}
	}

	inline void TextSprite::Clear()
	{
		m_atlases.clear();
		m_boundingVolume.MakeNull();
		m_localVertices.clear();
		m_renderInfos.clear();
	}

	inline const Color& TextSprite::GetColor() const
	{
		return m_color;
	}

	inline const MaterialRef& TextSprite::GetMaterial() const
	{
		return m_material;
	}

	inline float TextSprite::GetScale() const
	{
		return m_scale;
	}

	inline void TextSprite::SetColor(const Color& color)
	{
		m_color = color;

		InvalidateVertices();
	}

	inline void TextSprite::SetDefaultMaterial()
	{
		MaterialRef material = Material::New();
		material->Enable(RendererParameter_Blend, true);
		material->Enable(RendererParameter_DepthWrite, false);
		material->Enable(RendererParameter_FaceCulling, false);
		material->EnableLighting(false);
		material->SetDstBlend(BlendFunc_InvSrcAlpha);
		material->SetSrcBlend(BlendFunc_SrcAlpha);

		SetMaterial(material);
	}

	inline void TextSprite::SetMaterial(MaterialRef material)
	{
		m_material = std::move(material);
	}

	inline void TextSprite::SetScale(float scale)
	{
		m_scale = scale;

		InvalidateVertices();
	}

	inline void TextSprite::InvalidateVertices()
	{
		InvalidateInstanceData(0);
	}

	inline TextSprite& TextSprite::operator=(const TextSprite& text)
	{
		InstancedRenderable::operator=(text);

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
			const AbstractAtlas* atlas = it->first;
			AtlasSlots& atlasSlots = m_atlases[atlas];

			atlasSlots.clearSlot.Connect(atlas->OnAtlasCleared, this, &TextSprite::OnAtlasInvalidated);
			atlasSlots.layerChangeSlot.Connect(atlas->OnAtlasLayerChange, this, &TextSprite::OnAtlasLayerChange);
			atlasSlots.releaseSlot.Connect(atlas->OnAtlasRelease, this, &TextSprite::OnAtlasInvalidated);
		}

		InvalidateBoundingVolume();
		InvalidateVertices();

		return *this;
	}

	template<typename... Args>
	TextSpriteRef TextSprite::New(Args&&... args)
	{
		std::unique_ptr<TextSprite> object(new TextSprite(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
