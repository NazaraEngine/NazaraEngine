// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::GuillotineTextureAtlas
	* \brief Graphics class that represents an atlas texture for guillotine
	*/

	/*!
	* \brief Gets the underlying data storage
	* \return Value of the enumeration of the underlying data storage
	*/
	DataStoreFlags GuillotineTextureAtlas::GetStorage() const
	{
		return DataStorage::Hardware;
	}

	/*!
	* \brief Resizes the image
	* \return Updated texture
	*
	* \param oldImage Old image to resize
	* \param size New image size
	*
	* \remark Produces a NazaraError if resize failed
	*/

	std::shared_ptr<AbstractImage> GuillotineTextureAtlas::ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const
	{
		TextureInfo textureInfo;
		textureInfo.width = size.x;
		textureInfo.height = size.y;
		textureInfo.pixelFormat = PixelFormat::A8;
		textureInfo.type = ImageType::E2D;
		
		std::shared_ptr<Texture> newTexture = m_renderDevice.InstantiateTexture(textureInfo);
		if (!newTexture)
			return nullptr;

		if (oldImage)
		{
			return nullptr;
			/*const Texture& oldTexture = static_cast<const Texture&>(*oldImage);

			// Copy of old data
			///TODO: Copy from texture to texture
			Image image;
			if (!oldTexture->Download(&image))
			{
				NazaraError("Failed to download old texture");
				return nullptr;
			}

			if (!newTexture->Update(&image, Rectui(0, 0, image.GetWidth(), image.GetHeight())))
			{
				NazaraError("Failed to update texture");
				return nullptr;
			}*/
		}

		return newTexture;
	}
}
