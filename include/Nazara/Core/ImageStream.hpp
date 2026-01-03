// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_IMAGESTREAM_HPP
#define NAZARA_CORE_IMAGESTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <mutex>

namespace Nz
{
	struct ImageStreamParams : public ResourceParameters
	{
		bool IsValid() const;
	};

	class ImageStream;

	using ImageStreamLoader = ResourceLoader<ImageStream, ImageStreamParams>;

	class NAZARA_CORE_API ImageStream : public Resource
	{
		public:
			using Params = ImageStreamParams;

			ImageStream() = default;
			virtual ~ImageStream();

			virtual bool DecodeNextFrame(void* frameBuffer, Time* frameTime) = 0;

			virtual UInt64 GetFrameCount() const = 0;
			virtual PixelFormat GetPixelFormat() const = 0;
			virtual Vector2ui GetSize() const = 0;

			virtual void Seek(UInt64 frameIndex) = 0;

			virtual UInt64 Tell() = 0;

			static std::shared_ptr<ImageStream> OpenFromFile(const std::filesystem::path& filePath, const ImageStreamParams& params = ImageStreamParams());
			static std::shared_ptr<ImageStream> OpenFromMemory(const void* data, std::size_t size, const ImageStreamParams& params = ImageStreamParams());
			static std::shared_ptr<ImageStream> OpenFromStream(Stream& stream, const ImageStreamParams& params = ImageStreamParams());
	};
}

#include <Nazara/Core/ImageStream.inl>

#endif // NAZARA_CORE_IMAGESTREAM_HPP
