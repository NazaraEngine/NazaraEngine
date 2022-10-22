// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP
#define NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API ValuePropertyHandler
	{
		public:
			ValuePropertyHandler() = default;
			ValuePropertyHandler(const ValuePropertyHandler&) = delete;
			ValuePropertyHandler(ValuePropertyHandler&&) = delete;
			~ValuePropertyHandler() = default;

			ValuePropertyHandler& operator=(const ValuePropertyHandler&) = delete;
			ValuePropertyHandler& operator=(ValuePropertyHandler&&) = delete;

		private:
	};
}

#include <Nazara/Graphics/PropertyHandler/ValuePropertyHandler.inl>

#endif // NAZARA_GRAPHICS_PROPERTYHANDLER_VALUEPROPERTYHANDLER_HPP
