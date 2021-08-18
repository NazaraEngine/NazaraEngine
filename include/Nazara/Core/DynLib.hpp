// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <filesystem>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#define NAZARA_DYNLIB_EXTENSION ".dll"
#elif defined(NAZARA_PLATFORM_LINUX)
	#define NAZARA_DYNLIB_EXTENSION ".so"
#elif defined(NAZARA_PLATFORM_MACOSX)
	#define NAZARA_DYNLIB_EXTENSION ".dynlib"
#else
	#error OS not handled
#endif

namespace Nz
{
	using DynLibFunc = void (*)(void); // "Generic" type of pointer to function

	class DynLibImpl;

	class NAZARA_CORE_API DynLib
	{
		public:
			DynLib();
			DynLib(const DynLib&) = delete;
			DynLib(DynLib&&) noexcept;
			~DynLib();

			std::string GetLastError() const;
			DynLibFunc GetSymbol(const char* symbol) const;

			bool IsLoaded() const;

			bool Load(const std::filesystem::path& libraryPath);
			void Unload();

			DynLib& operator=(const DynLib&) = delete;
			DynLib& operator=(DynLib&& lib) noexcept;

		private:
			mutable std::string m_lastError;
			std::unique_ptr<DynLibImpl> m_impl;
	};
}

#endif // NAZARA_DYNLIB_HPP
