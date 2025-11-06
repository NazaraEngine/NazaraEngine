// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_ANDROID_ICONIMPL_HPP
#define NAZARA_PLATFORM_ANDROID_ICONIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Image.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	class IconImpl
	{
		public:
			IconImpl(const Image& image);
			IconImpl(const IconImpl&) = delete;
			IconImpl(IconImpl&&) noexcept = default;
			~IconImpl() = default;

			IconImpl& operator=(const IconImpl&) = default;
			IconImpl& operator=(IconImpl&&) noexcept = default;
	};
}

#endif // NAZARA_PLATFORM_ANDROID_ICONIMPL_HPP
