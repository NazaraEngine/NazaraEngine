// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/Debug.hpp>

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

#include <Nazara/Graphics/DebugOff.hpp>
