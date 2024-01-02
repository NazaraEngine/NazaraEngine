// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_DYNLIBIMPL_HPP
#define NAZARA_CORE_WIN32_DYNLIBIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <filesystem>
#include <Windows.h>

namespace Nz
{
	class DynLibImpl
	{
		public:
			DynLibImpl() = default;
			DynLibImpl(const DynLibImpl&) = delete;
			DynLibImpl(DynLibImpl&&) noexcept = default;
			~DynLibImpl();

			DynLibFunc GetSymbol(const char* symbol, std::string* errorMessage) const;
			bool Load(const std::filesystem::path& libraryPath, std::string* errorMessage);

			DynLibImpl& operator=(const DynLibImpl&) = delete;
			DynLibImpl& operator=(DynLibImpl&&) noexcept = default;

		private:
			MovablePtr<std::remove_pointer_t<HMODULE>> m_handle;
	};
}

#endif // NAZARA_CORE_WIN32_DYNLIBIMPL_HPP
