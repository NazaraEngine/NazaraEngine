// Copyright (C) 2013 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Debug.hpp>

NzDynLibImpl::NzDynLibImpl(NzDynLib* parent)
{
	NazaraUnused(parent);
}

NzDynLibFunc NzDynLibImpl::GetSymbol(const NzString& symbol, NzString* errorMessage) const
{
	/*
		Il n'est pas standard de cast un pointeur d'objet vers un pointeur de fonction.
		Nous devons donc utiliser des techniques diaboliques venant du malin lui-même.. :D
		Au moins ce n'est pas aussi terrible qu'un const_cast
		-Lynix
	*/
	union
	{
		NzDynLibFunc func;
		void* pointer;
	} converter;

	dlerror(); // Clear error flag

	converter.pointer = dlsym(m_handle, symbol.GetConstBuffer());
	if (!converter.pointer)
		*errorMessage = dlerror();

	return converter.func;
}

bool NzDynLibImpl::Load(const NzString& libraryPath, NzString* errorMessage)
{
	NzString path = libraryPath;
	
	unsigned int pos = path.FindLast(".so");
	if (pos == NzString::npos || (path.GetLength() > pos+3 && path[pos+3] != '.'))
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

void NzDynLibImpl::Unload()
{
	dlclose(m_handle);
}
