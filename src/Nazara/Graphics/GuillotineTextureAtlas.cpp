// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
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

	UInt32 GuillotineTextureAtlas::GetStorage() const
	{
		return DataStorage_Hardware;
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

	AbstractImage* GuillotineTextureAtlas::ResizeImage(AbstractImage* oldImage, const Vector2ui& size) const
	{
		std::unique_ptr<Texture> newTexture(new Texture);
		if (newTexture->Create(ImageType_2D, PixelFormatType_A8, size.x, size.y, 1))
		{
			if (oldImage)
			{
				Texture* oldTexture = static_cast<Texture*>(oldImage);

				// Copy of old data
				///TODO: Copy from texture to texture
				Image image;
				if (!oldTexture->Download(&image))
				{
					NazaraError("Failed to download old texture");
					return nullptr;
				}

				if (!newTexture->Update(image, Rectui(0, 0, image.GetWidth(), image.GetHeight())))
				{
					NazaraError("Failed to update texture");
					return nullptr;
				}
			}

			return newTexture.release();
		}
		else
		{
			// If we are here, it is that the size is too big for the graphic card or we don't have enough
			return nullptr;
		}
	}
}
