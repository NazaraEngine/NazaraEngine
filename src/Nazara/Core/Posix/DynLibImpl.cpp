// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/String.hpp>
#include <dlfcn.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DynLibImpl::DynLibImpl(DynLib* parent)
	{
		NazaraUnused(parent);
	}

	DynLibFunc DynLibImpl::GetSymbol(const String& symbol, String* errorMessage) const
	{
		/*
			Il n'est pas standard de cast un pointeur d'objet vers un pointeur de fonction.
			Nous devons donc utiliser des techniques diaboliques venant du malin lui-même.. :D
			Au moins ce n'est pas aussi terrible qu'un const_cast
			-Lynix
		*/
		union
		{
			DynLibFunc func;
			void* pointer;
		} converter;

		dlerror(); // Clear error flag

		converter.pointer = dlsym(m_handle, symbol.GetConstBuffer());
		if (!converter.pointer)
			*errorMessage = dlerror();

		return converter.func;
	}

	bool DynLibImpl::Load(const String& libraryPath, String* errorMessage)
	{
		String path = libraryPath;

		unsigned int pos = path.FindLast(".so");
		if (pos == String::npos || (path.GetLength() > pos+3 && path[pos+3] != '.'))
			path += ".so";

		dlerror(); // Clear error flag
		m_handle = dlopen(path.GetConstBuffer(), RTLD_LAZY | RTLD_GLOBAL);

		if (m_handle)
			return true;
		else
		{
			*errorMessage = dlerror();
			return false;
		}
	}

	void DynLibImpl::Unload()
	{
		dlclose(m_handle);
	}
}
