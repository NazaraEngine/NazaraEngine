// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBREQUESTRESULT_HPP
#define NAZARA_NETWORK_WEBREQUESTRESULT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Network/Export.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/Result.hpp>
#include <functional>
#include <string>

#ifndef NAZARA_PLATFORM_WEB
using CURL = void;
#else
struct emscripten_fetch_t;
#endif

namespace Nz
{
	class WebService;

	class NAZARA_NETWORK_API WebRequestResult
	{
		friend class WebRequest;

		public:
			WebRequestResult(const WebRequestResult&) = delete;
			WebRequestResult(WebRequestResult&&) = delete;
			~WebRequestResult() = default;

			inline std::string& GetBody();
			inline const std::string& GetBody() const;
			UInt64 GetDownloadedSize() const;
			UInt64 GetDownloadSpeed() const;
			inline const std::string& GetErrorMessage() const;
			UInt32 GetStatusCode() const;

			inline bool HasSucceeded() const;

			inline explicit operator bool() const;

			WebRequestResult& operator=(const WebRequestResult&) = delete;
			WebRequestResult& operator=(WebRequestResult&&) = delete;

		private:
#ifndef NAZARA_PLATFORM_WEB
			inline WebRequestResult(WebService& webService, Result<std::string, std::string>&& bodyResult, CURL* curl);
#else
			inline WebRequestResult(WebService& webService, Result<std::string, std::string>&& bodyResult, emscripten_fetch_t* fetchHandle, Time downloadTime);
#endif

#ifndef NAZARA_PLATFORM_WEB
			CURL* m_curlHandle;
#else
			emscripten_fetch_t* m_fetchHandle;
#endif
			WebService& m_webService;
			Result<std::string, std::string> m_bodyResult;
#ifdef NAZARA_PLATFORM_WEB
			Time m_downloadTime;
#endif

	};
}

#include <Nazara/Network/WebRequestResult.inl>

#endif // NAZARA_NETWORK_WEBREQUESTRESULT_HPP
