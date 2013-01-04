// Copyright (C) 2012 Jérôme Leclercq
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
	NzDynLibFunc sym = reinterpret_cast<NzDynLibFunc>(dlsym(m_handle, symbol.GetConstBuffer()));
	if (!sym)
		m_parent->SetLastError(NzGetLastSystemError()); // dlerror() ?

	return sym;
}

bool NzDynLibImpl::Load(const NzString& libraryPath)
{
	NzString path = libraryPath;
	if (!path.EndsWith(".so"))
		path += ".so";

    m_handle = dlopen(path.GetConstBuffer(),);

	if (m_handle)
		return true;
	else
	{
		m_parent->SetLastError(NzGetLastSystemError());
		return false;
	}
}

void NzDynLibImpl::Unload()
{
	dlclose(m_handle);
}
