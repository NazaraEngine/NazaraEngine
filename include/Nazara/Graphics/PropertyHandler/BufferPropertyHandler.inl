// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline BufferPropertyHandler::BufferPropertyHandler(std::string propertyName) :
	m_bufferTag(std::move(propertyName)),
	m_propertyName(m_bufferTag)
	{
	}

	inline BufferPropertyHandler::BufferPropertyHandler(std::string propertyName, std::string optionName) :
	m_bufferTag(std::move(propertyName)),
	m_optionName(std::move(optionName)),
	m_propertyName(m_bufferTag)
	{
	}

	inline Nz::BufferPropertyHandler::BufferPropertyHandler(std::string propertyName, std::string bufferTag, std::string optionName):
	m_bufferTag(std::move(bufferTag)),
	m_optionName(std::move(optionName)),
	m_propertyName(std::move(propertyName))
	{
	}
}
