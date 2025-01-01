// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Resource.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Resource
	* \brief Core class that represents a resource
	*/

	Resource::~Resource() = default;

	/*!
	* \brief Gets the file path associated with the resource
	* \return A reference to the path
	*/

	const std::filesystem::path& Resource::GetFilePath() const
	{
		return m_filePath;
	}

	/*!
	* \brief Sets the file path associated with the resource
	*
	* \param filePath Path to the resource
	*/

	void Resource::SetFilePath(std::filesystem::path filePath)
	{
		m_filePath = std::move(filePath);
	}
}
