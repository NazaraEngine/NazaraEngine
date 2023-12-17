// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_IMAGESTREAM_HPP
#define NAZARA_UTILITY_IMAGESTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Asset.hpp>
#include <Nazara/Core/AssetLoader.hpp>
#include <Nazara/Core/AssetParameters.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <mutex>

namespace Nz
{
	struct ImageStreamParams : public AssetParameters
	{
		bool IsValid() const;
	};

	class ImageStream;

	using ImageStreamLoader = AssetLoader<ImageStream, ImageStreamParams>;

	class NAZARA_UTILITY_API ImageStream : public Asset
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

#include <Nazara/Utility/ImageStream.inl>

#endif // NAZARA_UTILITY_IMAGESTREAM_HPP
