// Copyright (C) 2012 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Debug.hpp>

NzDynLibImpl::NzDynLibImpl(NzDynLib* parent) :
m_parent(parent)
{
}

NzDynLibFunc NzDynLibImpl::GetSymbol(const NzString& symbol) const
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

	converter.pointer = reinterpret_cast<NzDynLibFunc>(dlsym(m_handle, symbol.GetConstBuffer()));
	if (!converter.pointer)
		m_parent->SetLastError(dlerror());

	return converter.func;
}

bool NzDynLibImpl::Load(const NzString& libraryPath)
{
	NzString path = libraryPath;
	if (!path.EndsWith(".so"))
		path += ".so";

	dlerror(); // Clear error flag

	m_handle = dlopen(path.GetConstBuffer());

	if (m_handle)
		return true;
	else
	{
		m_parent->SetLastError(dlerror());
		return false;
	}
}

void NzDynLibImpl::Unload()
{
	dlclose(m_handle);
}
