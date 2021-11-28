// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_CLIPBOARD_HPP
#define NAZARA_PLATFORM_CLIPBOARD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <string>

namespace Nz
{
	class NAZARA_PLATFORM_API Clipboard
	{
		public:
			Clipboard() = delete;
			~Clipboard() = delete;

			static ClipboardContentType GetContentType();
			static std::string GetString();
			static void SetString(const std::string& str);
	};
}

#include <Nazara/Platform/Clipboard.inl>

#endif // NAZARA_PLATFORM_CLIPBOARD_HPP
