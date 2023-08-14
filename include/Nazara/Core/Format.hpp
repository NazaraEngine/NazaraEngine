// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_FORMAT_HPP
#define NAZARA_CORE_FORMAT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <string>

namespace Nz
{
	template<typename... Args> std::string Format(std::string_view str, Args&&... args);
}

#include <Nazara/Core/Format.inl>

#endif // NAZARA_CORE_FORMAT_HPP
