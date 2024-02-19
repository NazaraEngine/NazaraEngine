// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/CurlLibrary.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <NazaraUtils/CallOnExit.hpp>

namespace Nz
{
	bool CurlLibrary::Load()
	{
		Unload();

		CallOnExit unloadOnFailure([this] { Unload(); });

		auto PostLoad = [&]
		{
			if (global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
			{
				NazaraError("failed to initialize libcurl");
				return false;
			}
			m_isInitialized = true;

			unloadOnFailure.Reset();
			return true;
		};

#ifndef NAZARA_NETWORK_LIBCURL_LINK
		for (const char* libname : { "libcurl" NAZARA_DYNLIB_EXTENSION, "libcurl-d" NAZARA_DYNLIB_EXTENSION })
		{
			ErrorFlags errFlags(Nz::ErrorMode::Silent);
			if (m_library.Load(libname))
				break;
		}

		if (!m_library.IsLoaded())
		{
			NazaraErrorFmt("failed to load libcurl: {0}", m_library.GetLastError());
			return false;
		}

		auto LoadSymbol = [this](const char* name, bool optional)
		{
			DynLibFunc funcPtr = m_library.GetSymbol(name);
			if (!funcPtr && !optional)
				throw std::runtime_error(std::string("failed to load ") + name);

			return funcPtr;
		};

		try
		{
#define NAZARA_CURL_FUNCTION(name) name = reinterpret_cast<decltype(&::curl_##name)>(LoadSymbol("curl_" #name, false));
#include <Nazara/Network/CurlFunctions.hpp>
		}
		catch (const std::exception& e)
		{
			NazaraError(e.what());
			return false;
		}

		return PostLoad();
#else
		// libcurl is linked to the executable

#define NAZARA_CURL_FUNCTION(name) name = &::curl_##name;
#include <Nazara/Network/CurlFunctions.hpp>

		return PostLoad();
#endif
	}

	void CurlLibrary::Unload()
	{
		if (!m_library.IsLoaded())
			return;

		if (m_isInitialized)
			global_cleanup();

#define NAZARA_CURL_FUNCTION(name) name = nullptr;
#include <Nazara/Network/CurlFunctions.hpp>

		m_library.Unload();
	}
}
