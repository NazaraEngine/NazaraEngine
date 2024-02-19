// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline TexturePropertyHandler::TexturePropertyHandler(std::string propertyName) :
	m_propertyName(std::move(propertyName)),
	m_samplerTag(m_propertyName)
	{
	}

	inline TexturePropertyHandler::TexturePropertyHandler(std::string propertyName, std::string optionName) :
	m_optionName(std::move(optionName)),
	m_propertyName(std::move(propertyName)),
	m_samplerTag(m_propertyName)
	{
	}

	inline TexturePropertyHandler::TexturePropertyHandler(std::string propertyName, std::string samplerTag, std::string optionName) :
	m_optionName(std::move(optionName)),
	m_propertyName(std::move(propertyName)),
	m_samplerTag(std::move(samplerTag))
	{
	}
}

