// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/WebRequest.hpp>
#include <Nazara/Network/CurlLibrary.hpp> //< include last because of curl/curl.h
#include <Nazara/Network/WebService.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	WebRequest::WebRequest(WebService& webService) :
	m_webService(webService),
	m_isUserAgentSet(false)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		m_curlHandle = libcurl.easy_init();
	}

	WebRequest::~WebRequest()
	{
		auto& libcurl = m_webService.GetCurlLibrary();
		if (m_curlHandle)
			libcurl.easy_cleanup(m_curlHandle);

		if (m_headerList)
			libcurl.slist_free_all(m_headerList);
	}

	void WebRequest::ForceProtocol(Nz::NetProtocol protocol)
	{
		assert(protocol != Nz::NetProtocol::Unknown);

		auto& libcurl = m_webService.GetCurlLibrary();
		switch (protocol)
		{
			case Nz::NetProtocol::Any:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
				break;

			case Nz::NetProtocol::IPv4:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
				break;

			case Nz::NetProtocol::IPv6:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
				break;

			case Nz::NetProtocol::Unknown:
				break;
		}
	}

	void WebRequest::SetJSonContent(std::string_view encodedJSon)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		SetHeader("Content-Type", "application/json");
		libcurl.easy_setopt(m_curlHandle, CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(encodedJSon.size()));
		libcurl.easy_setopt(m_curlHandle, CURLOPT_COPYPOSTFIELDS, encodedJSon.data());
	}

	void WebRequest::SetMaximumFileSize(Nz::UInt64 maxFileSize)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t maxSize = maxFileSize;
		libcurl.easy_setopt(m_curlHandle, CURLOPT_MAXFILESIZE_LARGE, maxSize);
	}

	void WebRequest::SetServiceName(const std::string_view& serviceName)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		if (!serviceName.empty())
		{
			//TODO Nz::StackString?
			std::string userAgent = m_webService.GetUserAgent();
			userAgent.reserve(userAgent.size() + 3 + serviceName.size());
			userAgent.append(" - ");
			userAgent.append(serviceName.data(), serviceName.size());

			libcurl.easy_setopt(m_curlHandle, CURLOPT_USERAGENT, userAgent.data());
		}
		else
			libcurl.easy_setopt(m_curlHandle, CURLOPT_USERAGENT, m_webService.GetUserAgent().c_str());

		m_isUserAgentSet = true;
	}

	void WebRequest::SetURL(const std::string& url)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		libcurl.easy_setopt(m_curlHandle, CURLOPT_URL, url.data());
	}

	void WebRequest::SetupGet()
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		libcurl.easy_setopt(m_curlHandle, CURLOPT_HTTPGET, long(1));
	}

	void WebRequest::SetupPost()
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		libcurl.easy_setopt(m_curlHandle, CURLOPT_POST, long(1));
	}

	CURL* WebRequest::Prepare()
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		if (!m_headers.empty())
		{
			for (auto&& [header, value] : m_headers)
			{
				std::string headerValue = (!value.empty()) ? header + ": " + value : header + ";";
				m_headerList = libcurl.slist_append(m_headerList, headerValue.c_str());
			}

			libcurl.easy_setopt(m_curlHandle, CURLOPT_HTTPHEADER, m_headerList.Get());
		}

		if (!m_isUserAgentSet)
			libcurl.easy_setopt(m_curlHandle, CURLOPT_USERAGENT, m_webService.GetUserAgent().c_str());

		return m_curlHandle;
	}
}
