// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PropertyHandler::~PropertyHandler() = default;

	bool PropertyHandler::NeedsUpdateOnTextureUpdate(std::size_t /*updatedTexturePropertyIndex*/) const
	{
		return false;
	}

	bool PropertyHandler::NeedsUpdateOnValueUpdate(std::size_t /*updatedValuePropertyIndex*/) const
	{
		return false;
	}
}
