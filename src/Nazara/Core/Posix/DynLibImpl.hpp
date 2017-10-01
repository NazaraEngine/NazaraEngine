// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIBIMPL_HPP
#define NAZARA_DYNLIBIMPL_HPP

#include <Nazara/Core/DynLib.hpp>

namespace Nz
{
	class String;

	class DynLibImpl
	{
		public:
			DynLibImpl(DynLib* m_parent);
			~DynLibImpl() = default;

			DynLibFunc GetSymbol(const String& symbol, String* errorMessage) const;
			bool Load(const String& libraryPath, String* errorMessage);
			void Unload();

		private:
			void* m_handle;
	};
}

#endif // NAZARA_DYNLIBIMPL_HPP
