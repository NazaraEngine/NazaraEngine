// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Debug.hpp>

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

	const String& Resource::GetFilePath() const
	{
		return m_filePath;
	}

	/*!
	* \brief Sets the file path associated with the resource
	*
	* \param filePath Path to the resource
	*/

	void Resource::SetFilePath(const String& filePath)
	{
		m_filePath = filePath;
	}
}
