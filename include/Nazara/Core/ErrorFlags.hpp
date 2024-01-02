// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ERRORFLAGS_HPP
#define NAZARA_CORE_ERRORFLAGS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Error.hpp>

namespace Nz
{
	class ErrorFlags
	{
		public:
			inline ErrorFlags(ErrorModeFlags orFlags, ErrorModeFlags andFlags = {});
			ErrorFlags(const ErrorFlags&) = delete;
			ErrorFlags(ErrorFlags&&) = delete;
			inline ~ErrorFlags();

			inline ErrorModeFlags GetPreviousFlags() const;

			inline void SetFlags(ErrorModeFlags orFlags, ErrorModeFlags andFlags = {});

			ErrorFlags& operator=(const ErrorFlags&) = delete;
			ErrorFlags& operator=(ErrorFlags&&) = delete;

		private:
			ErrorModeFlags m_previousFlags;
	};
}

#include <Nazara/Core/ErrorFlags.inl>

#endif // NAZARA_CORE_ERRORFLAGS_HPP
