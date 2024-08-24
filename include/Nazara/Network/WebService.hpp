// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBSERVICE_HPP
#define NAZARA_NETWORK_WEBSERVICE_HPP

#include <Nazara/Network/Export.hpp>
#include <Nazara/Network/WebRequest.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <unordered_map>
#include <vector>

#ifndef NAZARA_PLATFORM_WEB
using CURLM = void;
#endif

namespace Nz
{
	class CurlLibrary;
	class Logger;

	class NAZARA_NETWORK_API WebService
	{
		friend WebRequest;
		friend WebRequestResult;

		public:
#ifndef NAZARA_PLATFORM_WEB
			WebService(const CurlLibrary& library);
#else
			WebService();
#endif
			WebService(const WebService&) = delete;
			WebService(WebService&&) = delete;
			~WebService();

			inline std::unique_ptr<WebRequest> AllocateRequest();

			inline std::unique_ptr<WebRequest> CreateRequest(WebRequestMethod method, const std::string& url, WebRequest::ResultCallback callback);

			inline const std::string& GetUserAgent() const;

			bool Poll();

			void QueueRequest(const FunctionRef<bool(WebRequest& request)>& builder);
			void QueueRequest(std::unique_ptr<WebRequest>&& request);

			WebService& operator=(const WebService&) = delete;
			WebService& operator=(WebService&&) = delete;

		private:
#ifndef NAZARA_PLATFORM_WEB
			inline const CurlLibrary& GetCurlLibrary() const;
#endif

			std::string m_userAgent;
#ifndef NAZARA_PLATFORM_WEB
			std::unordered_map<CURL*, std::unique_ptr<WebRequest>> m_activeRequests;
			const CurlLibrary& m_curl;
			MovablePtr<CURLM> m_curlMulti;
#else
			struct FinishedRequest
			{
				std::unique_ptr<WebRequest> request;
				bool succeeded;
			};

			std::unordered_map<emscripten_fetch_t*, std::unique_ptr<WebRequest>> m_activeRequests;
			std::vector<FinishedRequest> m_finishedRequests;
#endif
	};
}

#include <Nazara/Network/WebService.inl>

#endif // NAZARA_NETWORK_WEBSERVICE_HPP
