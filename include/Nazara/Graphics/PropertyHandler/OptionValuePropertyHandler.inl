// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline OptionValuePropertyHandler::OptionValuePropertyHandler(std::string propertyName, std::string optionName) :
	m_propertyName(std::move(propertyName)),
	m_optionName(std::move(optionName))
	{
	}
}
