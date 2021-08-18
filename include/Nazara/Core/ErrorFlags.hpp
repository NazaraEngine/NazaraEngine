// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERRORFLAGS_HPP
#define NAZARA_ERRORFLAGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>

namespace Nz
{
	class NAZARA_CORE_API ErrorFlags
	{
		public:
			ErrorFlags(ErrorModeFlags flags, bool replace = false);
			ErrorFlags(const ErrorFlags&) = delete;
			ErrorFlags(ErrorFlags&&) = delete;
			~ErrorFlags();

			ErrorModeFlags GetPreviousFlags() const;

			void SetFlags(ErrorModeFlags flags, bool replace = false);

			ErrorFlags& operator=(const ErrorFlags&) = delete;
			ErrorFlags& operator=(ErrorFlags&&) = delete;

		private:
			ErrorModeFlags m_previousFlags;
	};
}

#endif // NAZARA_ERRORFLAGS_HPP
