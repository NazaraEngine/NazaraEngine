// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILE_HPP
#define NAZARA_FILE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>
#include <ctime>
#include <filesystem>
#include <fstream>

namespace Nz
{
	class FileImpl;

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

			bool EndOfFile() const;
			bool EndOfStream() const override;

			bool Exists() const;

			UInt64 GetCursorPos() const override;
			std::filesystem::path GetDirectory() const override;
			std::filesystem::path GetFileName() const;
			std::filesystem::path GetPath() const override;
			UInt64 GetSize() const override;

			bool IsOpen() const;

			bool Open(OpenModeFlags openMode = OpenMode_NotOpen);
			bool Open(const std::filesystem::path& filePath, OpenModeFlags openMode = OpenMode_NotOpen);

			bool SetCursorPos(CursorPosition pos, Int64 offset = 0);
			bool SetCursorPos(UInt64 offset) override;
			bool SetFile(const std::filesystem::path& filePath);
			bool SetSize(UInt64 size);

			File& operator=(const File&) = delete;
			File& operator=(File&& file) noexcept;

			static inline ByteArray ComputeHash(HashType hash, const std::filesystem::path& filePath);
			static inline ByteArray ComputeHash(AbstractHash* hash, const std::filesystem::path& filePath);

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			std::filesystem::path m_filePath;
			std::unique_ptr<FileImpl> m_impl;
	};

	NAZARA_CORE_API bool HashAppend(AbstractHash* hash, const File& originalFile);
}

#include <Nazara/Core/File.inl>

#endif // NAZARA_FILE_HPP
