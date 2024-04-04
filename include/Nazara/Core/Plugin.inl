// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz
{
	template<typename T>
	Plugin<T>::Plugin(DynLib dynLib, std::unique_ptr<T> pluginInterface, bool isActive) :
	m_interface(std::move(pluginInterface)),
	m_lib(std::move(dynLib)),
	m_isActive(isActive)
	{
	}

	template<typename T>
	Plugin<T>::~Plugin()
	{
		if (m_interface)
		{
			Deactivate();

			// Destroys interface before freeing the library
			m_interface.reset();
		}

		m_lib.Unload();
	}

	template<typename T>
	bool Plugin<T>::Activate()
	{
		if (m_isActive)
			return true;

		if (!m_interface->Activate())
			return false;

		m_isActive = true;
		return true;
	}

	template<typename T>
	template<typename U>
	Plugin<U> Plugin<T>::Cast() &&
	{
		return Plugin<U>(std::move(m_lib), std::unique_ptr<U>(static_cast<U*>(m_interface.release())), m_isActive);
	}

	template<typename T>
	void Plugin<T>::Deactivate()
	{
		if (m_isActive)
		{
			m_interface->Deactivate();
			m_isActive = false;
		}
	}

	template<typename T>
	const DynLib& Plugin<T>::GetDynamicLibrary() const
	{
		return m_lib;
	}

	template<typename T>
	T& Plugin<T>::GetInterface()
	{
		return *m_interface;
	}

	template<typename T>
	const T& Plugin<T>::GetInterface() const
	{
		return *m_interface;
	}

	template<typename T>
	T* Plugin<T>::operator->()
	{
		return m_interface.get();
	}

	template<typename T>
	const T* Plugin<T>::operator->() const
	{
		return m_interface.get();
	}
}

