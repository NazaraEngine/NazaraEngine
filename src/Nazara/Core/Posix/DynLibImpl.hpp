// Copyright (C) 2014 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIBIMPL_HPP
#define NAZARA_DYNLIBIMPL_HPP

#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <dlfcn.h>

class NzString;

class NzDynLibImpl : NzNonCopyable
{
	public:
		NzDynLibImpl(NzDynLib* m_parent);
		~NzDynLibImpl() = default;

		NzDynLibFunc GetSymbol(const NzString& symbol) const;
		bool Load(const NzString& libraryPath);
		void Unload();

	private:
		void* m_handle;
};

#endif // NAZARA_DYNLIBIMPL_HPP
