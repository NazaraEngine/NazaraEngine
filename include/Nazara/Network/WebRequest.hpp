// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBREQUEST_HPP
#define NAZARA_NETWORK_WEBREQUEST_HPP

#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Enums.hpp>
#include <Nazara/Network/WebRequestResult.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <functional>
#include <string>
#include <unordered_map>

struct curl_slist;

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
			void SetJSonContent(std::string_view encodedJSon);
			void SetMaximumFileSize(UInt64 maxFileSize);
			inline void SetResultCallback(ResultCallback callback);
			void SetServiceName(const std::string_view& serviceName);
			void SetURL(const std::string& url);

			void SetupGet();
			void SetupPost();

			WebRequest& operator=(const WebRequest&) = delete;
			WebRequest& operator=(WebRequest&&) = default;

			static std::unique_ptr<WebRequest> Get(const std::string& url, ResultCallback callback = nullptr);
			static std::unique_ptr<WebRequest> Post(const std::string& url, ResultCallback callback = nullptr);

		private:
			inline bool OnBodyResponse(const char* data, std::size_t length);
			CURL* Prepare();
			inline void TriggerCallback();
			inline void TriggerCallback(std::string errorMessage);

			std::string m_responseBody;
			std::unordered_map<std::string, std::string> m_headers;
			WebService& m_webService;
			DataCallback m_dataCallback;
			MovablePtr<CURL> m_curlHandle;
			MovablePtr<curl_slist> m_headerList;
			ResultCallback m_resultCallback;
			bool m_isUserAgentSet;
	};
}

#include <Nazara/Network/WebRequest.inl>

#endif // NAZARA_NETWORK_WEBREQUEST_HPP
