// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Utility/Image.hpp>
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

	std::shared_ptr<Texture> Texture::CreateFromImage(const Image& image, const TextureParams& params)
	{
		NazaraAssert(params.IsValid(), "Invalid TextureParams");

		Nz::TextureInfo texParams;
		texParams.depth = image.GetDepth();
		texParams.height = image.GetHeight();
		texParams.pixelFormat = image.GetFormat();
		texParams.type = image.GetType();
		texParams.width = image.GetWidth();
		texParams.usageFlags = params.usageFlags;

		std::shared_ptr<Texture> texture = params.renderDevice->InstantiateTexture(texParams);
		if (!texture->Update(image.GetConstPixels()))
		{
			NazaraError("failed to update texture");
			return {};
		}

		texture->SetFilePath(image.GetFilePath());
		if (std::string debugName = image.GetFilePath().generic_u8string(); !debugName.empty())
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

	std::shared_ptr<Texture> Texture::LoadArrayFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadArrayFromFile(filePath, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadArrayFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadArrayFromMemory(data, size, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadArrayFromStream(Stream& stream, const TextureParams& textureParams, const Vector2ui& atlasSize)
	{
		std::shared_ptr<Image> image = Image::LoadArrayFromStream(stream, textureParams, atlasSize);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadCubemapFromFile(const std::filesystem::path& filePath, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadCubemapFromFile(filePath, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadCubemapFromMemory(const void* data, std::size_t size, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadCubemapFromMemory(data, size, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}

	std::shared_ptr<Texture> Texture::LoadCubemapFromStream(Stream& stream, const TextureParams& textureParams, const CubemapParams& cubemapParams)
	{
		std::shared_ptr<Image> image = Image::LoadCubemapFromStream(stream, textureParams, cubemapParams);
		if (!image)
			return {};

		return CreateFromImage(*image, textureParams);
	}
}
