// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Texture.hpp>

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
	std::shared_ptr<AbstractImage> GuillotineTextureAtlas::ResizeImage(const AbstractImage* oldImage, const Vector2ui& size) const
	{
		TextureInfo textureInfo;
		textureInfo.width = size.x;
		textureInfo.height = size.y;
		textureInfo.pixelFormat = m_texturePixelFormat;
		textureInfo.type = ImageType::E2D;
		textureInfo.usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferSource | TextureUsage::TransferDestination;
		textureInfo.levelCount = 1; //< No mipmaps for atlases

		std::shared_ptr<Texture> newTexture;
		try
		{
			newTexture = m_renderDevice.InstantiateTexture(textureInfo);
			m_renderDevice.Execute([&](CommandBufferBuilder& builder)
			{
				builder.TextureBarrier({ .srcStageMask = PipelineStage::TopOfPipe, .dstStageMask = PipelineStage::AllGraphicsCommands, .srcAccessMask = {}, .dstAccessMask = MemoryAccess::ShaderRead, .oldLayout = TextureLayout::Undefined, .newLayout = TextureLayout::ColorInput, .texture = newTexture.get() });
			}, QueueType::Graphics);
		}
		catch (const std::exception& e)
		{
			NazaraError("failed to instantiate texture: {0}", e.what());
			return nullptr;
		}

		if (oldImage)
		{
			const Texture& oldTexture = static_cast<const Texture&>(*oldImage);
			Vector3ui oldSize = oldTexture.GetSize();

			if (!newTexture->Copy(oldTexture, Boxui(0, 0, 0, oldSize.x, oldSize.y, oldSize.z)))
			{
				NazaraError("failed to update texture");
				return nullptr;
			}
		}

		return newTexture;
	}

	void GuillotineTextureAtlas::UpdateImage(AbstractImage& image, const void* ptr, const Rectui& rect, UInt32 srcWidth, UInt32 srcHeight) const
	{
		Texture& dstTexture = SafeCast<Texture&>(image);

		std::unique_ptr<AsyncRenderCommands> asyncTransfer = m_renderDevice.InstantiateAsyncCommands(QueueType::Graphics);
		asyncTransfer->AddCommands([&](Nz::CommandBufferBuilder& builder)
		{
			builder.TextureBarrier({ .srcStageMask = PipelineStage::BottomOfPipe, .dstStageMask = PipelineStage::AllGraphicsCommands, .srcAccessMask = {}, .dstAccessMask = MemoryAccess::TransferWrite, .oldLayout = TextureLayout::ColorInput, .newLayout = TextureLayout::TransferDestination, .texture = &dstTexture });
		});

		dstTexture.Update(*asyncTransfer, ptr, Boxui(rect.x, rect.y, 0, rect.width, rect.height, 1), srcWidth, srcHeight);

		asyncTransfer->AddCommands([&](Nz::CommandBufferBuilder& builder)
		{
			builder.TextureBarrier({ .srcStageMask = PipelineStage::Transfer, .dstStageMask = PipelineStage::AllGraphicsCommands, .srcAccessMask = MemoryAccess::TransferWrite, .dstAccessMask = MemoryAccess::ShaderRead, .oldLayout = TextureLayout::TransferDestination, .newLayout = TextureLayout::ColorInput, .texture = &dstTexture });
		});

		m_renderDevice.SubmitAsyncCommands(std::move(asyncTransfer), true);
	}
}
