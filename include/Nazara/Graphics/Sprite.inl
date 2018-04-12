// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Sprite object by default
	*/

	inline Sprite::Sprite() :
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f),
	m_origin(Nz::Vector3f::Zero())
	{
		ResetMaterials(1);

		for (Color& color : m_cornerColor)
			color = Color::White;

		SetDefaultMaterial();
	}

	/*!
	* \brief Constructs a Sprite object with a reference to a material
	*
	* \param material Reference to a material
	*/
	inline Sprite::Sprite(MaterialRef material) :
	Sprite()
	{
		SetMaterial(std::move(material), true);
	}

	/*!
	* \brief Constructs a Sprite object with a pointer to a texture
	*
	* \param texture Pointer to a texture
	*/

	inline Sprite::Sprite(Texture* texture) :
	Sprite()
	{
		SetTexture(texture, true);
	}

	/*!
	* \brief Constructs a Sprite object by assignation
	*
	* \param sprite Sprite to copy into this
	*/

	inline Sprite::Sprite(const Sprite& sprite) :
	InstancedRenderable(sprite),
	m_color(sprite.m_color),
	m_textureCoords(sprite.m_textureCoords),
	m_size(sprite.m_size),
	m_origin(sprite.m_origin)
	{
	}

	/*!
	* \brief Gets the color of the sprite
	*
	* This is the global color of the sprite, independent from corner colors
	*
	* \return Current color
	*
	* \see GetCornerColor
	* \see SetColor
	*/
	inline const Color& Sprite::GetColor() const
	{
		return m_color;
	}

	/*!
	* \brief Gets the color setup on a corner of the sprite
	*
	* \return Current color
	*
	* \param corner Corner of the sprite to query
	*
	* \see SetCornerColor
	*/
	inline const Color& Sprite::GetCornerColor(RectCorner corner) const
	{
		NazaraAssert(static_cast<std::size_t>(corner) < m_cornerColor.size(), "Invalid corner");

		return m_cornerColor[corner];
	}

	/*!
	* \brief Gets the origin of the sprite
	*
	* \return Current material
	*
	* \see SetOrigin
	*/
	inline const Vector3f& Sprite::GetOrigin() const
	{
		return m_origin;
	}

	/*!
	* \brief Gets the size of the sprite
	* \return Current size
	*/

	inline const Vector2f& Sprite::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Gets the texture coordinates of the sprite
	* \return Current texture coordinates
	*/
	inline const Rectf& Sprite::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	/*!
	* \brief Sets the global color of the sprite
	*
	* This is independent from the corner color of the sprite
	*
	* \param color Color for the sprite
	*
	* \see GetColor
	* \see SetCornerColor
	*/
	inline void Sprite::SetColor(const Color& color)
	{
		m_color = color;

		InvalidateVertices();
	}

	/*!
	* \brief Sets a color for a corner of the sprite
	*
	* This is independent from the sprite global color, which gets multiplied by the corner color when rendering the sprite.
	*
	* \param corner Corner of the sprite to set
	* \param color Color for the sprite
	*
	* \see GetCornerColor
	* \see SetColor
	*/
	inline void Sprite::SetCornerColor(RectCorner corner, const Color& color)
	{
		NazaraAssert(static_cast<std::size_t>(corner) < m_cornerColor.size(), "Invalid corner");

		m_cornerColor[corner] = color;

		InvalidateVertices();
	}

	/*!
	* \brief Sets the default material of the sprite (just default material)
	*/

	inline void Sprite::SetDefaultMaterial()
	{
		MaterialRef material = Material::New();
		material->EnableFaceCulling(false);
		material->EnableScissorTest(true);

		SetMaterial(std::move(material));
	}

	/*!
	* \brief Changes the material of the sprite
	*
	* \param material Material for the sprite
	* \param resizeSprite Should billboard be resized to the material size (diffuse map)
	*/
	inline void Sprite::SetMaterial(MaterialRef material, bool resizeSprite)
	{
		SetMaterial(GetSkin(), std::move(material), resizeSprite);
	}

	/*!
	* \brief Sets the material of the sprite
	*
	* \param skinIndex Skin index to change
	* \param material Material for the sprite
	* \param resizeBillboard Should billboard be resized to the material size (diffuse map)
	*/
	inline void Sprite::SetMaterial(std::size_t skinIndex, MaterialRef material, bool resizeSprite)
	{
		InstancedRenderable::SetMaterial(skinIndex, 0, std::move(material));

		if (resizeSprite)
		{
			if (const MaterialRef& newMat = GetMaterial())
			{
				const TextureRef& diffuseMap = newMat->GetDiffuseMap();
				if (diffuseMap && diffuseMap->IsValid())
					SetSize(Vector2f(Vector2ui(diffuseMap->GetSize())));
			}
		}
	}

	/*!
	* \brief Sets the origin of the sprite
	*
	* The origin is the center of translation/rotation/scaling of the sprite.
	*
	* \param origin New origin for the sprite
	*
	* \see GetOrigin
	*/
	inline void Sprite::SetOrigin(const Vector3f& origin)
	{
		m_origin = origin;

		// On invalide la bounding box
		InvalidateBoundingVolume();
		InvalidateVertices();
	}

	/*!
	* \brief Sets the size of the sprite
	*
	* \param size Size for the sprite
	*/

	inline void Sprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		// On invalide la bounding box
		InvalidateBoundingVolume();
		InvalidateVertices();
	}

	/*!
	* \brief Sets the size of the sprite
	*
	* \param sizeX Size in X for the sprite
	* \param sizeY Size in Y for the sprite
	*/

	inline void Sprite::SetSize(float sizeX, float sizeY)
	{
		SetSize(Vector2f(sizeX, sizeY));
	}

	/*!
	* \brief Sets the texture of the sprite for the current skin
	*
	* This function changes the diffuse map of the material associated with the current skin
	*
	* \param texture Texture for the sprite
	* \param resizeSprite Should the sprite be resized to the texture size?
	*
	* \remark The sprite material gets copied to prevent accidentally changing other drawable materials
	*/
	inline void Sprite::SetTexture(TextureRef texture, bool resizeSprite)
	{
		SetTexture(GetSkin(), std::move(texture), resizeSprite);
	}

	/*!
	* \brief Sets the texture of the sprite for a specific skin
	*
	* This function changes the diffuse map of the material associated with the specified skin
	*
	* \param skinIndex Skin index to change
	* \param texture Texture for the sprite
	* \param resizeSprite Should the sprite be resized to the texture size?
	*
	* \remark The sprite material gets copied to prevent accidentally changing other drawable materials
	*/
	inline void Sprite::SetTexture(std::size_t skinIndex, TextureRef texture, bool resizeSprite)
	{
		const MaterialRef& material = GetMaterial(skinIndex);

		if (material->GetReferenceCount() > 1)
		{
			MaterialRef newMat = Material::New(*material); // Copy
			newMat->SetDiffuseMap(std::move(texture));

			SetMaterial(skinIndex, std::move(newMat), resizeSprite);
		}
		else
		{
			material->SetDiffuseMap(std::move(texture));
			if (resizeSprite && texture && texture->IsValid())
				SetSize(Vector2f(Vector2ui(texture->GetSize())));
		}
	}

	/*!
	* \brief Sets the texture coordinates of the sprite
	*
	* \param coords Texture coordinates
	*/

	inline void Sprite::SetTextureCoords(const Rectf& coords)
	{
		m_textureCoords = coords;

		InvalidateVertices();
	}

	/*!
	* \brief Sets the texture rectangle of the sprite
	*
	* \param rect Rectangles symbolizing the size of the texture
	*
	* \remark Produces a NazaraAssert if material is invalid
	* \remark Produces a NazaraAssert if material has no diffuse map
	*/

	inline void Sprite::SetTextureRect(const Rectui& rect)
	{
		const MaterialRef& material = GetMaterial();
		NazaraAssert(material->HasDiffuseMap(), "Sprite material has no diffuse map");

		Texture* diffuseMap = material->GetDiffuseMap();

		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		SetTextureCoords(Rectf(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height));
	}

	/*!
	* \brief Sets the current sprite with the content of the other one
	* \return A reference to this
	*
	* \param sprite The other Sprite
	*/

	inline Sprite& Sprite::operator=(const Sprite& sprite)
	{
		InstancedRenderable::operator=(sprite);

		m_color = sprite.m_color;
		m_origin = sprite.m_origin;
		m_textureCoords = sprite.m_textureCoords;
		m_size = sprite.m_size;

		// We do not copy final vertices because it's highly probable that our parameters are modified and they must be regenerated
		InvalidateBoundingVolume();
		InvalidateVertices();

		return *this;
	}

	/*!
	* \brief Invalidates the vertices
	*/

	inline void Sprite::InvalidateVertices()
	{
		InvalidateInstanceData(0);
	}

	/*!
	* \brief Creates a new sprite from the arguments
	* \return A reference to the newly created sprite
	*
	* \param args Arguments for the sprite
	*/

	template<typename... Args>
	SpriteRef Sprite::New(Args&&... args)
	{
		std::unique_ptr<Sprite> object(new Sprite(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
