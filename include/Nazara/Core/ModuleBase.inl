// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Log.hpp>

namespace Nz
{
	template<typename T>
	ModuleBase<T>::ModuleBase(std::string moduleName, T* pointer) :
	ModuleBase(std::move(moduleName), pointer, NoLog{})
	{
		LogInit();
	}

	template<typename T>
	ModuleBase<T>::ModuleBase(std::string moduleName, T* pointer, NoLog) :
	m_moduleName(std::move(moduleName))
	{
		NazaraAssertFmt(T::s_instance == nullptr, "only one instance of {} can exist at a given time", m_moduleName);
		T::s_instance = pointer;
	}

	template<typename T>
	ModuleBase<T>::~ModuleBase()
	{
		LogUninit();
		T::s_instance = nullptr;
	}

	template<typename T>
	T* ModuleBase<T>::Instance()
	{
		return T::s_instance;
	}

	template<typename T>
	void ModuleBase<T>::LogInit()
	{
		NazaraNotice("Initializing {0}...", m_moduleName);
	}

	template<typename T>
	void ModuleBase<T>::LogUninit()
	{
		if (m_moduleName.empty())
			return;

		NazaraNotice("Uninitialized {0}", m_moduleName);
		m_moduleName.clear();
	}
}

