// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIBIMPL_HPP
#define NAZARA_DYNLIBIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <windows.h>

class NzString;

class NzDynLibImpl
{
	public:
		NzDynLibImpl(NzDynLib* m_parent);
		NzDynLibImpl(const NzDynLibImpl&) = delete;
		NzDynLibImpl(NzDynLibImpl&&) = delete; ///TODO?
		~NzDynLibImpl() = default;

		NzDynLibFunc GetSymbol(const NzString& symbol, NzString* errorMessage) const;
		bool Load(const NzString& libraryPath, NzString* errorMessage);
		void Unload();

		NzDynLibImpl& operator=(const NzDynLibImpl&) = delete;
		NzDynLibImpl& operator=(NzDynLibImpl&&) = delete; ///TODO?

	private:
		HMODULE m_handle;
};

#endif // NAZARA_DYNLIBIMPL_HPP
