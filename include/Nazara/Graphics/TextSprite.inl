// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a TextSprite object by default
	*/

	inline TextSprite::TextSprite() :
	m_color(Color::White),
	m_scale(1.f)
	{
		SetDefaultMaterial();
	}

	/*!
	* \brief Constructs a TextSprite object with a drawer
	*
	* \param drawer Drawer used to compose text on the sprite
	*/

	inline TextSprite::TextSprite(const AbstractTextDrawer& drawer) :
	TextSprite()
	{
		Update(drawer);
	}

	/*!
	* \brief Constructs a TextSprite object by assignation
	*
	* \param sprite TextSprite to copy into this
	*/

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

	/*!
	* \brief Clears the data
	*/

	inline void TextSprite::Clear()
	{
		m_atlases.clear();
		m_boundingVolume.MakeNull();
		m_localVertices.clear();
		m_renderInfos.clear();
	}

	/*!
	* \brief Gets the color of the text sprite
	* \return Current color
	*/

	inline const Color& TextSprite::GetColor() const
	{
		return m_color;
	}

	/*!
	* \brief Gets the material of the text sprite
	* \return Current material
	*/

	inline const MaterialRef& TextSprite::GetMaterial() const
	{
		return m_material;
	}

	/*!
	* \brief Gets the current scale of the text sprite
	* \return Current scale
	*/

	inline float TextSprite::GetScale() const
	{
		return m_scale;
	}

	/*!
	* \brief Sets the color of the text sprite
	*
	* \param color Color for the text sprite
	*/

	inline void TextSprite::SetColor(const Color& color)
	{
		m_color = color;

		InvalidateVertices();
	}

	/*!
	* \brief Sets the default material of the text sprite (just default material)
	*/


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

	/*!
	* \brief Sets the material of the text sprite
	*
	* \param material Material for the text sprite
	*/

	inline void TextSprite::SetMaterial(MaterialRef material)
	{
		m_material = std::move(material);
	}

	/*!
	* \brief Sets the current scale of the text sprite
	*
	* \param scale Scale of the text sprite
	*/

	inline void TextSprite::SetScale(float scale)
	{
		m_scale = scale;

		InvalidateVertices();
	}

	/*!
	* \brief Sets the current text sprite with the content of the other one
	* \return A reference to this
	*
	* \param text sprite The other TextSprite
	*/

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

	/*!
	* \brief Invalidates the vertices
	*/

	inline void TextSprite::InvalidateVertices()
	{
		InvalidateInstanceData(0);
	}

	/*!
	* \brief Creates a new text sprite from the arguments
	* \return A reference to the newly created text sprite
	*
	* \param args Arguments for the text sprite
	*/

	template<typename... Args>
	TextSpriteRef TextSprite::New(Args&&... args)
	{
		std::unique_ptr<TextSprite> object(new TextSprite(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
