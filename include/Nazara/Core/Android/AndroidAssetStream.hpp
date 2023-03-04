// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ANDROID_ANDROIDASSETSTREAM_HPP
#define NAZARA_CORE_ANDROID_ANDROIDASSETSTREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Stream.hpp>
#include <android/asset_manager.h>

namespace Nz
{
	class NAZARA_CORE_API AndroidAssetStream final : public Stream
	{
		public:
			inline AndroidAssetStream(std::string filePath, AAsset* asset);
			AndroidAssetStream(const AndroidAssetStream&) = delete;
			AndroidAssetStream(AndroidAssetStream&&) = delete;
			~AndroidAssetStream();

			UInt64 GetSize() const override;

			AndroidAssetStream& operator=(const AndroidAssetStream&) = delete;
			AndroidAssetStream& operator=(AndroidAssetStream&&) = delete;

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			bool SeekStreamCursor(UInt64 offset) override;
			UInt64 TellStreamCursor() const override;
			bool TestStreamEnd() const override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			std::string m_filePath;
			AAsset* m_asset;
	};
}

#include <Nazara/Core/Android/AndroidAssetStream.inl>

#endif // NAZARA_CORE_ANDROID_ANDROIDASSETSTREAM_HPP
