// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_CLIPBOARD_HPP
#define NAZARA_PLATFORM_CLIPBOARD_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <Nazara/Platform/Export.hpp>
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
