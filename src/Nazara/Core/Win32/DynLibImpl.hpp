// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIBIMPL_HPP
#define NAZARA_DYNLIBIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <windows.h>

namespace Nz
{
	class String;

	class DynLibImpl
	{
		public:
			DynLibImpl(DynLib* m_parent);
			DynLibImpl(const DynLibImpl&) = delete;
			DynLibImpl(DynLibImpl&&) = delete; ///TODO?
			~DynLibImpl() = default;

			DynLibFunc GetSymbol(const String& symbol, String* errorMessage) const;
			bool Load(const String& libraryPath, String* errorMessage);
			void Unload();

			DynLibImpl& operator=(const DynLibImpl&) = delete;
			DynLibImpl& operator=(DynLibImpl&&) = delete; ///TODO?

		private:
			HMODULE m_handle;
	};
}

#endif // NAZARA_DYNLIBIMPL_HPP
