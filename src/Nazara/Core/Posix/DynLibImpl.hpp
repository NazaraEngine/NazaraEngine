// Copyright (C) 2024 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_DYNLIBIMPL_HPP
#define NAZARA_CORE_POSIX_DYNLIBIMPL_HPP

#include <Nazara/Core/DynLib.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <filesystem>
#include <string>

namespace Nz
{
	class String;

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
			MovablePtr<void> m_handle;
	};
}

#endif // NAZARA_CORE_POSIX_DYNLIBIMPL_HPP
