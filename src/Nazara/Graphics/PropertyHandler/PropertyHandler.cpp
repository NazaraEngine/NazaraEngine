// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PropertyHandler/PropertyHandler.hpp>

namespace Nz
{
	PropertyHandler::~PropertyHandler() = default;

	bool PropertyHandler::NeedsUpdateOnStorageBufferUpdate(std::size_t /*updatedStorageBufferPropertyIndex*/) const
	{
		return false;
	}

	bool PropertyHandler::NeedsUpdateOnTextureUpdate(std::size_t /*updatedTexturePropertyIndex*/) const
	{
		return false;
	}

	bool PropertyHandler::NeedsUpdateOnValueUpdate(std::size_t /*updatedValuePropertyIndex*/) const
	{
		return false;
	}
}
