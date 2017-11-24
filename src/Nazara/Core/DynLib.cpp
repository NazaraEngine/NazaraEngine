// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/DynLibImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/DynLibImpl.hpp>
#else
	#error No implementation for this platform
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DYNLIB
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::DynLib
	* \brief Core class that represents a dynamic library loader
	*/

	/*!
	* \brief Constructs a DynLib object by default
	*/

	DynLib::DynLib() :
	m_impl(nullptr)
	{
	}

	/*!
	* \brief Destructs the object and calls Unload
	*
	* \see Unload
	*/

	DynLib::~DynLib()
	{
		Unload();
	}

	/*!
	* \brief Gets the last error
	* \return Last error
	*/

	String DynLib::GetLastError() const
	{
		NazaraLock(m_mutex)

		return m_lastError;
	}

	/*!
	* \brief Gets the symbol for the name
	* \return Function which is the symbol of the function name
	*
	* \remark Produces a NazaraError if library is not loaded with NAZARA_CORE_SAFE defined
	*/

	DynLibFunc DynLib::GetSymbol(const String& symbol) const
	{
		NazaraLock(m_mutex)

		#if NAZARA_CORE_SAFE
		if (!IsLoaded())
		{
			NazaraError("Library not opened");
			return nullptr;
		}
		#endif

		return m_impl->GetSymbol(symbol, &m_lastError);
	}

	/*!
	* \brief Checks whether the library is loaded
	* \return true if loaded
	*/

	bool DynLib::IsLoaded() const
	{
		return m_impl != nullptr;
	}

	/*!
	* \brief Loads the library with that path
	* \return true if loading is successful
	*
	* \param libraryPath Path of the library
	*
	* \remark Produces a NazaraError if library is could not be loaded
	*/

	bool DynLib::Load(const String& libraryPath)
	{
		NazaraLock(m_mutex)

		Unload();

		std::unique_ptr<DynLibImpl> impl(new DynLibImpl(this));
		if (!impl->Load(libraryPath, &m_lastError))
		{
			NazaraError("Failed to load library: " + m_lastError);
			return false;
		}

		m_impl = impl.release();

		return true;
	}

	/*!
	* \brief Unloads the library
	*/

	void DynLib::Unload()
	{
		NazaraLock(m_mutex)

		if (IsLoaded())
		{
			m_impl->Unload();
			delete m_impl;
			m_impl = nullptr;
		}
	}
}
