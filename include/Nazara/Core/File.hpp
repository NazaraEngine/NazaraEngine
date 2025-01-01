// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FILE_HPP
#define NAZARA_CORE_FILE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <NazaraUtils/Result.hpp>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <optional>

namespace Nz
{
	namespace PlatformImpl
	{
		class FileImpl;
	}

	class NAZARA_CORE_API File : public Stream
	{
		public:
			File();
			File(const std::filesystem::path& filePath);
			File(const std::filesystem::path& filePath, OpenModeFlags openMode);
			File(const File&) = delete;
			File(File&& file) noexcept;
			~File();

			void Copy(const std::filesystem::path& newFilePath);
			void Close();

			bool Delete();

			bool Exists() const;

			std::filesystem::path GetDirectory() const override;
			std::filesystem::path GetFileName() const;
			std::filesystem::path GetPath() const override;
			UInt64 GetSize() const override;

			bool IsOpen() const;

			bool Open(OpenModeFlags openMode = OpenMode::NotOpen);
			bool Open(const std::filesystem::path& filePath, OpenModeFlags openMode = OpenMode::NotOpen);

			bool SetFile(const std::filesystem::path& filePath);
			bool SetSize(UInt64 size);

			File& operator=(const File&) = delete;
			File& operator=(File&& file) noexcept;

			static inline ByteArray ComputeHash(HashType hash, const std::filesystem::path& filePath);
			static inline ByteArray ComputeHash(AbstractHash& hash, const std::filesystem::path& filePath);
			static /*constexpr*/ Result<OpenModeFlags, std::string> DecodeOpenMode(std::string_view openModeStr);
			static std::optional<std::vector<UInt8>> ReadWhole(const std::filesystem::path& path);
			static bool WriteWhole(const std::filesystem::path& path, const void* data, std::size_t size);

		private:
			inline bool CheckFileOpening();
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			bool SeekStreamCursor(UInt64 offset) override;
			UInt64 TellStreamCursor() const override;
			bool TestStreamEnd() const override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			std::filesystem::path m_filePath;
			std::unique_ptr<PlatformImpl::FileImpl> m_impl;
	};

	NAZARA_CORE_API bool HashAppend(AbstractHash& hash, const File& originalFile);
}

#include <Nazara/Core/File.inl>

#endif // NAZARA_CORE_FILE_HPP
