// Copyright (C) 2025 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_FILEIMPL_HPP
#define NAZARA_CORE_POSIX_FILEIMPL_HPP

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <ctime>
#include <filesystem>

namespace Nz
{
	class File;
}

namespace Nz::PlatformImpl
{
	class FileImpl
	{
		public:
			FileImpl(const File* parent);
			FileImpl(const FileImpl&) = delete;
			FileImpl(FileImpl&&) = delete; ///TODO
			~FileImpl();

			bool EndOfFile() const;
			void Flush();
			UInt64 GetCursorPos() const;
			bool Open(const std::filesystem::path& filePath, OpenModeFlags mode);
			std::size_t Read(void* buffer, std::size_t size);
			bool SetCursorPos(CursorPosition pos, Int64 offset);
			bool SetSize(UInt64 size);
			std::size_t Write(const void* buffer, std::size_t size);

			FileImpl& operator=(const FileImpl&) = delete;
			FileImpl& operator=(FileImpl&&) = delete; ///TODO

		private:
			int m_fileDescriptor;
			mutable bool m_endOfFile;
			mutable bool m_endOfFileUpdated;
	};
}

#endif // NAZARA_CORE_POSIX_FILEIMPL_HPP
