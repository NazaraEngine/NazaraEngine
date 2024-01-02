// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBREQUEST_HPP
#define NAZARA_NETWORK_WEBREQUEST_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Enums.hpp>
#include <Nazara/Network/WebRequestResult.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <functional>
#include <string>
#include <unordered_map>

#ifndef NAZARA_PLATFORM_WEB
struct curl_slist;
#else
struct emscripten_fetch_attr_t;
#endif

namespace Nz
{
	class WebService;

	class NAZARA_NETWORK_API WebRequest
	{
		friend WebService;

		public:
			using DataCallback = std::function<bool(const void* data, std::size_t length)>;
			using ResultCallback = std::function<void(WebRequestResult&& result)>;

			WebRequest(WebService& owner);
			WebRequest(const WebRequest&) = delete;
			WebRequest(WebRequest&&) = default;
			~WebRequest();

			void ForceProtocol(NetProtocol protocol);

			inline void SetDataCallback(DataCallback callback);
			inline void SetHeader(std::string header, std::string value);
			void SetJSonContent(std::string encodedJSon);
			void SetMaximumFileSize(UInt64 maxFileSize);
			inline void SetResultCallback(ResultCallback callback);
			void SetServiceName(std::string serviceName);
			void SetURL(const std::string& url);

			void SetupGet();
			void SetupPost();

			WebRequest& operator=(const WebRequest&) = delete;
			WebRequest& operator=(WebRequest&&) = default;

		private:
			inline bool OnBodyResponse(const char* data, std::size_t length);
#ifndef NAZARA_PLATFORM_WEB
			CURL* Prepare();
#else
			inline emscripten_fetch_t* GetFetchHandle() const;
			inline Nz::Time GetRequestTime() const;
			emscripten_fetch_t* Prepare(emscripten_fetch_attr_t* fetchAttr);
			inline void StopClock();
#endif
			inline void TriggerErrorCallback(std::string errorMessage);
			inline void TriggerSuccessCallback();

#ifdef NAZARA_PLATFORM_WEB
			std::string m_httpMethod;
			std::string m_url;
			std::vector<const char*> m_requestHeaders;
#endif
			std::string m_content;
			std::string m_responseBody;
			std::unordered_map<std::string, std::string> m_headers;
			WebService& m_webService;
			DataCallback m_dataCallback;
#ifndef NAZARA_PLATFORM_WEB
			MovablePtr<CURL> m_curlHandle;
			MovablePtr<curl_slist> m_headerList;
#else
			HighPrecisionClock m_clock;
			MovablePtr<emscripten_fetch_t> m_fetchHandle;
#endif
			ResultCallback m_resultCallback;
			bool m_isUserAgentSet;
	};
}

#include <Nazara/Network/WebRequest.inl>

#endif // NAZARA_NETWORK_WEBREQUEST_HPP
