// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_SDL3_MESSAGEBOXIMPL_HPP
#define NAZARA_PLATFORM_SDL3_MESSAGEBOXIMPL_HPP

#include <NazaraUtils/Result.hpp>

namespace Nz
{
	class MessageBox;
	class Window;

	class MessageBoxImpl
	{
		public:
			static Result<int, const char*> Show(const MessageBox& messageBox, const Window* parent);
	};
}

#endif // NAZARA_PLATFORM_SDL3_MESSAGEBOXIMPL_HPP
