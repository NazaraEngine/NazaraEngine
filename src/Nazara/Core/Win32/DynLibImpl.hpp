// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_DYNLIBIMPL_HPP
#define NAZARA_CORE_WIN32_DYNLIBIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <filesystem>
#include <windows.h>

namespace Nz
{
	class DynLibImpl
	{
		public:
			DynLibImpl(DynLib* m_parent);
			DynLibImpl(const DynLibImpl&) = delete;
			DynLibImpl(DynLibImpl&&) = delete; ///TODO?
			~DynLibImpl();

			DynLibFunc GetSymbol(const char* symbol, std::string* errorMessage) const;
			bool Load(const std::filesystem::path& libraryPath, std::string* errorMessage);

			DynLibImpl& operator=(const DynLibImpl&) = delete;
			DynLibImpl& operator=(DynLibImpl&&) = delete; ///TODO?

		private:
			HMODULE m_handle;
	};
}

#endif // NAZARA_CORE_WIN32_DYNLIBIMPL_HPP
