// Copyright (C) 2020 Jérôme Leclercq
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

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::DynLib
	* \brief Core class that represents a dynamic library loader
	*/

	DynLib::DynLib() = default;
	DynLib::DynLib(DynLib&&) noexcept = default;
	DynLib::~DynLib() = default;

	/*!
	* \brief Gets the last error
	* \return Last error
	*/
	std::string DynLib::GetLastError() const
	{
		return m_lastError;
	}

	/*!
	* \brief Gets the symbol for the name
	* \return Function which is the symbol of the function name
	*
	* \remark Produces a NazaraError if library is not loaded with NAZARA_CORE_SAFE defined
	*/
	DynLibFunc DynLib::GetSymbol(const char* symbol) const
	{
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
	bool DynLib::Load(const std::filesystem::path& libraryPath)
	{
		Unload();

		auto impl = std::make_unique<DynLibImpl>(this);
		if (!impl->Load(libraryPath, &m_lastError))
		{
			NazaraError("Failed to load library: " + m_lastError);
			return false;
		}

		m_impl = std::move(impl);
		return true;
	}

	/*!
	* \brief Unloads the library
	*/
	void DynLib::Unload()
	{
		m_impl.reset();
	}

	DynLib& DynLib::operator=(DynLib&&) noexcept = default;
}
