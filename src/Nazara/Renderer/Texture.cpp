// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	Texture::~Texture() = default;

	bool TextureParams::IsValid() const
	{
		if (!ImageParams::IsValid())
			return false;

		if (!renderDevice)
		{
			NazaraError("no render device set");
			return false;
		}

		if (!usageFlags)
		{
			NazaraError("a texture should have at least one usage flag");
			return false;
		}

		return true;
	}

	void TextureParams::Merge(const TextureParams& params)
	{
		ImageParams::Merge(params);

		if (!renderDevice)
			renderDevice = params.renderDevice;

		usageFlags |= params.usageFlags;
	}

	std::shared_ptr<Texture> Texture::CreateFromImage(const Image& image, const TextureParams& params)
	{
		NazaraAssert(params.IsValid(), "Invalid TextureParams");

		TextureInfo texParams;
		texParams.pixelFormat = image.GetFormat();
		texParams.type = image.GetType();
		texParams.width = image.GetWidth();
		texParams.usageFlags = params.usageFlags;

		switch (image.GetType())
		{
			case ImageType::E1D:
				texParams.height = 1;
				break;

			case ImageType::E2D:
				texParams.height = image.GetHeight();
				break;

			case ImageType::E3D:
				texParams.height = image.GetHeight();
				texParams.depth = image.GetDepth();
				break;

			case ImageType::E1D_Array:
				texParams.height = 1;
				texParams.layerCount = image.GetHeight();
				break;

			case ImageType::E2D_Array:
				texParams.height = image.GetHeight();
				texParams.layerCount = image.GetDepth();
				break;

			case ImageType::Cubemap:
				texParams.height = image.GetHeight();
				texParams.layerCount = 6;
				break;
		}

		if (!params.buildMipmaps)
			texParams.levelCount = image.GetLevelCount();

		std::shared_ptr<Texture> texture = params.renderDevice->InstantiateTexture(texParams, image.GetConstPixels(), params.buildMipmaps);

		texture->SetFilePath(image.GetFilePath());
		if (std::string debugName = PathToString(texture->GetFilePath()); !debugName.empty())
			texture->UpdateDebugName(debugName);

		return texture;
	}

	std::shared_ptr<Texture> Texture::LoadFromFile(const std::filesystem::path& filePath, const TextureParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, params);
		if (!image)
			return {};

		return CreateFromImage(*image, params);
	}

	std::shared_ptr<Texture> Texture::LoadFromMemory(const void* data, std::size_t size, const TextureParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, params);
		if (!image)
			return {};

		return CreateFromImage(*image, params);
	}

	std::shared_ptr<Texture> Texture::LoadFromStream(Stream& stream, const TextureParams& params)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, params);
		if (!image)
			return {};

		return CreateFromImage(*image, params);
	}

	std::shared_ptr<Texture> Texture::LoadFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadFromStream(Stream& stream, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromFile(filePath, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromMemory(data, size, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadFromStream(Stream& stream, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadFromStream(stream, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}
}
