// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
		textureInfo.pixelFormat = PixelFormat::R8;
		textureInfo.type = ImageType::E2D;
		textureInfo.usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferSource | TextureUsage::TransferDestination;
		
		std::shared_ptr<Texture> newTexture;
		try
		{
			newTexture = m_renderDevice.InstantiateTexture(textureInfo);
		}
		catch (const std::exception& e)
		{
			NazaraError(std::string("Failed to instantiate texture: ") + e.what());
			return nullptr;
		}

		if (oldImage)
		{
			const Texture& oldTexture = static_cast<const Texture&>(*oldImage);
			Vector3ui oldSize = oldTexture.GetSize();

			if (!newTexture->Copy(oldTexture, Boxui(0, 0, 0, oldSize.x, oldSize.y, oldSize.z)))
			{
				NazaraError("Failed to update texture");
				return nullptr;
			}
		}

		return newTexture;
	}
}
