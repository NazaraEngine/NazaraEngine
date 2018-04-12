// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Sprite
	* \brief Graphics class that represents the rendering of a sprite
	*/

	/*!
	* \brief Adds the sprite to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/

	void Sprite::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const
	{
		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());
		renderQueue->AddSprites(instanceData.renderOrder, GetMaterial(), vertices, 1, scissorRect);
	}

	/*!
	* \brief Makes the bounding volume of this text
	*/

	void Sprite::MakeBoundingVolume() const
	{
		Vector3f origin(m_origin.x, -m_origin.y, m_origin.z);

		m_boundingVolume.Set(-origin, m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down() - origin);
	}

	/*!
	* \brief Sets the material of the sprite from a name
	*
	* Tries to get a material from the MaterialLibrary and then the MaterialManager (which will treat the name as a path)
	* Fails if the texture name is not a part of the MaterialLibrary nor the MaterialManager (which fails if it couldn't load the texture from its filepath)
	*
	* \param materialName Named texture for the material
	* \param resizeSprite Should the sprite be resized to the material diffuse map size?
	*
	* \return True if the material was found or loaded from its name/path, false if it couldn't
	*/
	bool Sprite::SetMaterial(String materialName, bool resizeSprite)
	{
		MaterialRef material = MaterialLibrary::Query(materialName);
		if (!material)
		{
			material = MaterialManager::Get(materialName);
			if (!material)
			{
				NazaraError("Failed to get material \"" + materialName + "\"");
				return false;
			}
		}

		SetMaterial(std::move(material), resizeSprite);
		return true;
	}

	/*!
	* \brief Sets the material of the sprite from a name for a specific skin
	*
	* Tries to get a material from the MaterialLibrary and then the MaterialManager (which will treat the name as a path)
	* Fails if the texture name is not a part of the MaterialLibrary nor the MaterialManager (which fails if it couldn't load the texture from its filepath)
	*
	* \param skinIndex Skin index to change
	* \param materialName Named texture for the material
	* \param resizeSprite Should the sprite be resized to the material diffuse map size?
	*
	* \return True if the material was found or loaded from its name/path, false if it couldn't
	*/
	bool Sprite::SetMaterial(std::size_t skinIndex, String materialName, bool resizeSprite)
	{
		MaterialRef material = MaterialLibrary::Query(materialName);
		if (!material)
		{
			material = MaterialManager::Get(materialName);
			if (!material)
			{
				NazaraError("Failed to get material \"" + materialName + "\"");
				return false;
			}
		}

		SetMaterial(skinIndex, std::move(material), resizeSprite);
		return true;
	}

	/*!
	* \brief Sets the texture of the sprite from a name for the current skin
	* \return True if the texture was found or loaded from its name/path, false if it couldn't
	*
	* Tries to get a texture from the TextureLibrary and then the TextureManager (which will treat the name as a path)
	* Fails if the texture name is not a part of the TextureLibrary nor the TextureManager (which fails if it couldn't load the texture from its filepath)
	*
	* \param textureName Named texture for the sprite
	* \param resizeSprite Should the sprite be resized to the texture size?
	*
	* \remark The sprite material gets copied to prevent accidentally changing other drawable materials
	*/
	bool Sprite::SetTexture(String textureName, bool resizeSprite)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get texture \"" + textureName + "\"");
				return false;
			}
		}

		SetTexture(std::move(texture), resizeSprite);
		return true;
	}

	/*!
	* \brief Sets the texture of the sprite from a name for a specific skin
	* \return True if the texture was found or loaded from its name/path, false if it couldn't
	*
	* Tries to get a texture from the TextureLibrary and then the TextureManager (which will treat the name as a path)
	* Fails if the texture name is not a part of the TextureLibrary nor the TextureManager (which fails if it couldn't load the texture from its filepath)
	*
	* \param skinIndex Named texture for the sprite
	* \param textureName Named texture for the sprite
	* \param resizeSprite Should the sprite be resized to the texture size?
	*
	* \remark The sprite material gets copied to prevent accidentally changing other drawable materials
	*/
	bool Sprite::SetTexture(std::size_t skinIndex, String textureName, bool resizeSprite)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get texture \"" + textureName + "\"");
				return false;
			}
		}

		SetTexture(skinIndex, std::move(texture), resizeSprite);
		return true;
	}

	/*!
	* \brief Updates the data of the sprite
	*
	* \param instanceData Data of the instance
	*/
	void Sprite::UpdateData(InstanceData* instanceData) const
	{
		instanceData->data.resize(4 * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		SparsePtr<Color> colorPtr(&vertices[0].color, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector3f> posPtr(&vertices[0].position, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector2f> texCoordPtr(&vertices[0].uv, sizeof(VertexStruct_XYZ_Color_UV));

		Vector3f origin(m_origin.x, -m_origin.y, m_origin.z);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_LeftTop];
		*posPtr++ = instanceData->transformMatrix.Transform(Vector3f(-origin));
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftTop);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_RightTop];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightTop);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_LeftBottom];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.y*Vector3f::Down() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftBottom);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_RightBottom];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightBottom);
	}

	/*!
	* \brief Initializes the sprite library
	* \return true If successful
	*
	* \remark Produces a NazaraError if the sprite library failed to be initialized
	*/

	bool Sprite::Initialize()
	{
		if (!SpriteLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the sprite library
	*/

	void Sprite::Uninitialize()
	{
		SpriteLibrary::Uninitialize();
	}

	SpriteLibrary::LibraryMap Sprite::s_library;
}
