// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_HPP
#define NAZARA_TEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct TextureInfo
	{
		PixelFormat pixelFormat;
		ImageType type;
		TextureUsageFlags usageFlags = TextureUsage::ShaderSampling | TextureUsage::TransferDestination;
		UInt8 mipmapLevel = 1;
		unsigned int depth = 1;
		unsigned int height;
		unsigned int width;
	};

	class NAZARA_RENDERER_API Texture : public Resource
	{
		public:
			Texture() = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) = delete;
			virtual ~Texture();

			virtual PixelFormat GetFormat() const = 0;
			virtual UInt8 GetLevelCount() const = 0;
			virtual Vector3ui GetSize(UInt8 level = 0) const = 0;
			virtual ImageType GetType() const = 0;

			virtual bool Update(const void* ptr) = 0;

			static inline unsigned int GetLevelSize(unsigned int size, unsigned int level);

			Texture& operator=(const Texture&) = delete;
			Texture& operator=(Texture&&) = delete;
	};
}

#include <Nazara/Renderer/Texture.inl>

#endif // NAZARA_TEXTURE_HPP
