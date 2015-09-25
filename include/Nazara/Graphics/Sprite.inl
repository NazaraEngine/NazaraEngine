// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline Sprite::Sprite() :
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f)
	{
		SetDefaultMaterial();
	}

	inline Sprite::Sprite(MaterialRef material) :
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f)
	{
		SetMaterial(std::move(material), true);
	}

	inline Sprite::Sprite(Texture* texture) :
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f)
	{
		SetTexture(texture, true);
	}

	inline Sprite::Sprite(const Sprite& sprite) :
	InstancedRenderable(sprite),
	m_color(sprite.m_color),
	m_material(sprite.m_material),
	m_textureCoords(sprite.m_textureCoords),
	m_size(sprite.m_size)
	{
	}

	inline const Color& Sprite::GetColor() const
	{
		return m_color;
	}

	inline const MaterialRef& Sprite::GetMaterial() const
	{
		return m_material;
	}

	inline const Vector2f& Sprite::GetSize() const
	{
		return m_size;
	}

	inline const Rectf& Sprite::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline void Sprite::SetColor(const Color& color)
	{
		m_color = color;

		InvalidateVertices();
	}

	inline void Sprite::SetDefaultMaterial()
	{
		MaterialRef material = Material::New();
		material->Enable(RendererParameter_FaceCulling, false);
		material->EnableLighting(false);

		SetMaterial(std::move(material));
	}

	inline void Sprite::SetMaterial(MaterialRef material, bool resizeSprite)
	{
		m_material = std::move(material);
		if (m_material && resizeSprite)
		{
			Texture* diffuseMap = m_material->GetDiffuseMap();
			if (diffuseMap && diffuseMap->IsValid())
				SetSize(Vector2f(Vector2ui(diffuseMap->GetSize())));
		}
	}

	inline void Sprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		// On invalide la bounding box
		InvalidateBoundingVolume();
		InvalidateVertices();
	}

	inline void Sprite::SetSize(float sizeX, float sizeY)
	{
		SetSize(Vector2f(sizeX, sizeY));
	}

	inline void Sprite::SetTexture(TextureRef texture, bool resizeSprite)
	{
		if (!m_material)
			SetDefaultMaterial();
		else if (m_material->GetReferenceCount() > 1)
			m_material = Material::New(*m_material); // Copie

		if (resizeSprite && texture && texture->IsValid())
			SetSize(Vector2f(Vector2ui(texture->GetSize())));

		m_material->SetDiffuseMap(std::move(texture));
	}

	inline void Sprite::SetTextureCoords(const Rectf& coords)
	{
		m_textureCoords = coords;
		InvalidateVertices();
	}

	inline void Sprite::SetTextureRect(const Rectui& rect)
	{
		NazaraAssert(m_material, "Sprite has no material");
		NazaraAssert(m_material->HasDiffuseMap(), "Sprite material has no diffuse map");

		Texture* diffuseMap = m_material->GetDiffuseMap();

		float invWidth = 1.f/diffuseMap->GetWidth();
		float invHeight = 1.f/diffuseMap->GetHeight();

		SetTextureCoords(Rectf(invWidth*rect.x, invHeight*rect.y, invWidth*rect.width, invHeight*rect.height));
	}

	inline Sprite& Sprite::operator=(const Sprite& sprite)
	{
		InstancedRenderable::operator=(sprite);

		m_color = sprite.m_color;
		m_material = sprite.m_material;
		m_textureCoords = sprite.m_textureCoords;
		m_size = sprite.m_size;

		// On ne copie pas les sommets finaux car il est très probable que nos paramètres soient modifiés et qu'ils doivent être régénérés de toute façon
		InvalidateBoundingVolume();
		InvalidateVertices();

		return *this;
	}

	inline void Sprite::InvalidateVertices()
	{
		InvalidateInstanceData(0);
	}

	template<typename... Args>
	SpriteRef Sprite::New(Args&&... args)
	{
		std::unique_ptr<Sprite> object(new Sprite(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
