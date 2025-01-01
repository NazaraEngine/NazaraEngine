// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline UniformValuePropertyHandler::UniformValuePropertyHandler(std::string propertyName, std::string blockTag) :
	m_blockTag(std::move(blockTag)),
	m_propertyName(std::move(propertyName)),
	m_memberTag(m_propertyName)
	{
	}

	inline UniformValuePropertyHandler::UniformValuePropertyHandler(std::string propertyName, std::string memberTag, std::string blockTag) :
	m_blockTag(std::move(blockTag)),
	m_propertyName(std::move(propertyName)),
	m_memberTag(std::move(memberTag))
	{
	}
}
