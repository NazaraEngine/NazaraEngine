// Copyright (C) 2021 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIBIMPL_HPP
#define NAZARA_DYNLIBIMPL_HPP

#include <Nazara/Core/DynLib.hpp>
#include <filesystem>
#include <string>

namespace Nz
{
	class String;

	class DynLibImpl
	{
		public:
			DynLibImpl(DynLib* m_parent);
			~DynLibImpl();

			DynLibFunc GetSymbol(const char* symbol, std::string* errorMessage) const;
			bool Load(const std::filesystem::path& libraryPath, std::string* errorMessage);

		private:
			void* m_handle;
	};
}

#endif // NAZARA_DYNLIBIMPL_HPP
