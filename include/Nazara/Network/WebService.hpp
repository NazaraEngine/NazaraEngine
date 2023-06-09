// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBSERVICE_HPP
#define NAZARA_NETWORK_WEBSERVICE_HPP

#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/WebRequest.hpp>
#include <NazaraUtils/MovablePtr.hpp>

using CURLM = void;

namespace Nz
{
	class CurlLibrary;
	class Logger;

	class NAZARA_NETWORK_API WebService
	{
		friend WebRequest;
		friend WebRequestResult;

		public:
			WebService(const CurlLibrary& library);
			WebService(const WebService&) = delete;
			WebService(WebService&&) = delete;
			~WebService();

			inline std::unique_ptr<WebRequest> AllocateRequest();

			inline std::unique_ptr<WebRequest> CreateGetRequest(const std::string& url, WebRequest::ResultCallback callback);
			inline std::unique_ptr<WebRequest> CreatePostRequest(const std::string& url, WebRequest::ResultCallback callback);

			inline const std::string& GetUserAgent() const;

			void Poll();

			void QueueRequest(std::unique_ptr<WebRequest>&& request);

			WebService& operator=(const WebService&) = delete;
			WebService& operator=(WebService&&) = delete;

		private:
			inline const CurlLibrary& GetCurlLibrary() const;

			std::string m_userAgent;
			std::unordered_map<CURL*, std::unique_ptr<WebRequest>> m_activeRequests;
			const CurlLibrary& m_curl;
			MovablePtr<CURLM> m_curlMulti;
	};
}

#include <Nazara/Network/WebService.inl>

#endif // NAZARA_NETWORK_WEBSERVICE_HPP
