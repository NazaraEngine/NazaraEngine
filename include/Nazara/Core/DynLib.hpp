// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_DYNLIB_HPP
#define NAZARA_CORE_DYNLIB_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <filesystem>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#define NAZARA_DYNLIB_EXTENSION ".dll"
#elif defined(NAZARA_PLATFORM_LINUX)
	#define NAZARA_DYNLIB_EXTENSION ".so"
#elif defined(NAZARA_PLATFORM_WEB)
	#define NAZARA_DYNLIB_EXTENSION ".wasm"
#elif defined(NAZARA_PLATFORM_MACOS)
	#define NAZARA_DYNLIB_EXTENSION ".dylib"
#else
	#error OS not handled
#endif

namespace Nz
{
	using DynLibFunc = void (*)(void); // "Generic" type of pointer to function

	namespace PlatformImpl
	{
		class DynLibImpl;
	}

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

			bool Load(std::filesystem::path libraryPath);
			void Unload();

			DynLib& operator=(const DynLib&) = delete;
			DynLib& operator=(DynLib&& lib) noexcept;

		private:
			mutable std::string m_lastError;
			std::unique_ptr<PlatformImpl::DynLibImpl> m_impl;
	};
}

#endif // NAZARA_CORE_DYNLIB_HPP
