// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_CURLLIBRARY_HPP
#define NAZARA_NETWORK_CURLLIBRARY_HPP

#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Network/Config.hpp>
#include <curl/curl.h>

namespace Nz
{
	class NAZARA_NETWORK_API CurlLibrary
	{
		public:
			inline CurlLibrary();
			CurlLibrary(const CurlLibrary&) = delete;
			CurlLibrary(CurlLibrary&&) = delete;
			inline ~CurlLibrary();

			inline bool IsLoaded() const;

			bool Load();

			void Unload();

			CurlLibrary& operator=(const CurlLibrary&) = delete;
			CurlLibrary& operator=(CurlLibrary&&) = delete;

#define NAZARA_CURL_FUNCTION(name) decltype(&::curl_##name) name;
#include <Nazara/Network/CurlFunctions.hpp>

		private:
			DynLib m_library;
			bool m_isInitialized;
	};
}

#include <Nazara/Network/CurlLibrary.inl>

#endif // NAZARA_NETWORK_CURLLIBRARY_HPP
