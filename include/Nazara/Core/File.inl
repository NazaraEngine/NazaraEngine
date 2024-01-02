// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Computes the hash for the file
	* \return ByteArray represing the result of the hash of the file
	*
	* \param hash Hash to execute
	* \param filePath Path for the file
	*/

	inline ByteArray File::ComputeHash(HashType hash, const std::filesystem::path& filePath)
	{
		return ComputeHash(*AbstractHash::Get(hash), filePath);
	}

	/*!
	* \brief Computes the hash for the file
	* \return ByteArray represing the result of the hash of the file
	*
	* \param hash Hash to execute
	* \param filePath Path for the file
	*/

	inline ByteArray File::ComputeHash(AbstractHash& hash, const std::filesystem::path& filePath)
	{
		return Nz::ComputeHash(hash, File(filePath));
	}

	inline /*constexpr*/ Result<OpenModeFlags, std::string> File::DecodeOpenMode(std::string_view openModeStr)
	{
		OpenModeFlags openModes = 0;
		
		if (openModeStr.find('r') != std::string_view::npos)
		{
			openModes |= OpenMode::Read;
			if (openModeStr.find('+') != std::string_view::npos)
				openModes |= OpenMode_ReadWrite | OpenMode::MustExist;
		}
		else if (openModeStr.find('w') != std::string_view::npos)
		{
			openModes |= OpenMode::Write | OpenMode::Truncate;
			if (openModeStr.find('+') != std::string_view::npos)
				openModes |= OpenMode::Read;
		}
		else if (openModeStr.find('a') != std::string_view::npos)
		{
			openModes |= OpenMode::Write | OpenMode::Append;
			if (openModeStr.find('+') != std::string_view::npos)
				openModes |= OpenMode::Read;
		}
		else
			return Err(Format("unhandled or invalid openmode {0}", openModeStr));

		if (openModeStr.find('b') == std::string_view::npos)
			openModes |= OpenMode::Text;

		return openModes;
	}

	inline bool File::CheckFileOpening()
	{
		if (m_openMode.Test(OpenMode::Defer))
		{
			if (!Open(m_filePath, m_openMode & ~OpenMode::Defer))
			{
				NazaraError("failed to open file");
				return false;
			}
		}

		return true;
	}
}

#include <Nazara/Core/DebugOff.hpp>
