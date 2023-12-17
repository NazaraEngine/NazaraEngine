// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Asset.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Resource
	* \brief Core class that represents a resource
	*/

	Asset::~Asset() = default;

	/*!
	* \brief Gets the file path associated with the resource
	* \return A reference to the path
	*/

	const std::filesystem::path& Asset::GetFilePath() const
	{
		return m_filePath;
	}

	/*!
	* \brief Sets the file path associated with the resource
	*
	* \param filePath Path to the resource
	*/

	void Asset::SetFilePath(std::filesystem::path filePath)
	{
		m_filePath = std::move(filePath);
	}
}
