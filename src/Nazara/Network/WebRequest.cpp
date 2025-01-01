// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/WebRequest.hpp>
#include <Nazara/Network/WebService.hpp>
#include <cstring>

#ifndef NAZARA_PLATFORM_WEB
#include <Nazara/Network/CurlLibrary.hpp>
#else
#include <emscripten/fetch.h>
#endif

namespace Nz
{
#ifndef NAZARA_PLATFORM_WEB
	WebRequest::WebRequest(WebService& webService) :
	m_webService(webService),
	m_isUserAgentSet(false)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		m_curlHandle = libcurl.easy_init();
	}
#else
	WebRequest::WebRequest(WebService& webService) :
	m_webService(webService),
	m_isUserAgentSet(false)
	{
	}
#endif

	WebRequest::~WebRequest()
	{
#ifndef NAZARA_PLATFORM_WEB
		auto& libcurl = m_webService.GetCurlLibrary();
		if (m_curlHandle)
			libcurl.easy_cleanup(m_curlHandle);

		if (m_headerList)
			libcurl.slist_free_all(m_headerList);
#else
		if (m_fetchHandle)
			emscripten_fetch_close(m_fetchHandle);
#endif
	}

#ifndef NAZARA_PLATFORM_WEB
	void WebRequest::ForceProtocol(Nz::NetProtocol protocol)
	{
		assert(protocol != Nz::NetProtocol::Unknown);

		auto& libcurl = m_webService.GetCurlLibrary();
		switch (protocol)
		{
			case Nz::NetProtocol::Any:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, long(CURL_IPRESOLVE_WHATEVER));
				break;

			case Nz::NetProtocol::IPv4:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, long(CURL_IPRESOLVE_V4));
				break;

			case Nz::NetProtocol::IPv6:
				libcurl.easy_setopt(m_curlHandle, CURLOPT_IPRESOLVE, long(CURL_IPRESOLVE_V6));
				break;

			case Nz::NetProtocol::Unknown:
				break;
		}
	}
#else
	void WebRequest::ForceProtocol(Nz::NetProtocol /*protocol*/)
	{
		// Ignored
	}
#endif

	void WebRequest::SetJSonContent(std::string encodedJSon)
	{
		SetHeader("Content-Type", "application/json");

		m_content = std::move(encodedJSon);

#ifndef NAZARA_PLATFORM_WEB
		auto& libcurl = m_webService.GetCurlLibrary();
		libcurl.easy_setopt(m_curlHandle, CURLOPT_POSTFIELDSIZE_LARGE, SafeCast<curl_off_t>(m_content.size()));
		libcurl.easy_setopt(m_curlHandle, CURLOPT_POSTFIELDS, m_content.data());
#endif
	}

#ifndef NAZARA_PLATFORM_WEB
	void WebRequest::SetMaximumFileSize(UInt64 maxFileSize)
	{
		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t maxSize = SafeCaster(maxFileSize);
		libcurl.easy_setopt(m_curlHandle, CURLOPT_MAXFILESIZE_LARGE, maxSize);
	}
#else
	void WebRequest::SetMaximumFileSize(UInt64 maxFileSize)
	{
		// TODO: Implement using EMSCRIPTEN_FETCH_STREAM_DATA
	}
#endif

	void WebRequest::SetMethod(WebRequestMethod method)
	{
		switch (method)
		{
			case WebRequestMethod::Delete: return SetMethod("DELETE");
			case WebRequestMethod::Head:   return SetMethod("HEAD");
			case WebRequestMethod::Get:
			{
#ifndef NAZARA_PLATFORM_WEB
				auto& libcurl = m_webService.GetCurlLibrary();
				libcurl.easy_setopt(m_curlHandle, CURLOPT_HTTPGET, long(1));
				libcurl.easy_setopt(m_curlHandle, CURLOPT_POST, long(0));
#else
				SetMethod("GET");
#endif
				break;
			}

			case WebRequestMethod::Patch:  return SetMethod("PATCH");
			case WebRequestMethod::Put:    return SetMethod("PUT");
			case WebRequestMethod::Post:
			{
#ifndef NAZARA_PLATFORM_WEB
				auto& libcurl = m_webService.GetCurlLibrary();
				libcurl.easy_setopt(m_curlHandle, CURLOPT_HTTPGET, long(0));
				libcurl.easy_setopt(m_curlHandle, CURLOPT_POST, long(1));
#else
				SetMethod("POST");
#endif
				break;
			}
		}
	}

	void WebRequest::SetMethod(std::string customMethod)
	{
		m_httpMethod = std::move(customMethod);
#ifndef NAZARA_PLATFORM_WEB
		auto& libcurl = m_webService.GetCurlLibrary();
		libcurl.easy_setopt(m_curlHandle, CURLOPT_CUSTOMREQUEST, m_httpMethod.data());
#endif
	}

	void WebRequest::SetServiceName(std::string serviceName)
	{
		if (!serviceName.empty())
		{
			//TODO Nz::StackString?
			std::string userAgent = m_webService.GetUserAgent();
			userAgent.reserve(userAgent.size() + 3 + serviceName.size());
			userAgent.append(" - ");
			userAgent.append(serviceName.data(), serviceName.size());

			SetHeader("User-Agent", std::move(userAgent));
		}
		else
			SetHeader("User-Agent", m_webService.GetUserAgent());

		m_isUserAgentSet = true;
	}

	void WebRequest::SetURL(const std::string& url)
	{
#ifndef NAZARA_PLATFORM_WEB
		auto& libcurl = m_webService.GetCurlLibrary();
		libcurl.easy_setopt(m_curlHandle, CURLOPT_URL, url.data());
#else
		m_url = url;
#endif
	}

#ifndef NAZARA_PLATFORM_WEB
	CURL* WebRequest::Prepare()
	{
		if (!m_isUserAgentSet)
			SetHeader("User-Agent", m_webService.GetUserAgent());

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
			SetHeader("User-Agent", m_webService.GetUserAgent());

		if (m_progressCallback)
		{
			curl_xferinfo_callback progressCallback = [](void* userdata, curl_off_t dltotal, curl_off_t dlnow, curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) -> int
			{
				WebRequest* request = static_cast<WebRequest*>(userdata);
				if (!request->m_progressCallback(SafeCast<UInt64>(dlnow), SafeCast<UInt64>(dltotal)))
					return -1;

				return 0; //< don't return CURL_PROGRESSFUNC_CONTINUE as it would output progress on stdout
			};

			libcurl.easy_setopt(m_curlHandle, CURLOPT_NOPROGRESS, long(0));
			libcurl.easy_setopt(m_curlHandle, CURLOPT_XFERINFODATA, this);
			libcurl.easy_setopt(m_curlHandle, CURLOPT_XFERINFOFUNCTION, progressCallback);
		}

		if (m_options.Test(WebRequestOption::FailOnError))
			libcurl.easy_setopt(m_curlHandle, CURLOPT_FAILONERROR, long(1));

		if (m_options.Test(WebRequestOption::FollowRedirects))
			libcurl.easy_setopt(m_curlHandle, CURLOPT_FOLLOWLOCATION, long(1));

		return m_curlHandle;
	}
#else
	emscripten_fetch_t* WebRequest::Prepare(emscripten_fetch_attr_t* fetchAttr)
	{
		if (!m_isUserAgentSet)
			SetHeader("User-Agent", m_webService.GetUserAgent());

		if (m_httpMethod.size() >= Nz::CountOf(fetchAttr->requestMethod))
			throw std::runtime_error("request method is too big");

		if (m_url.empty())
			throw std::runtime_error("no url set");

		std::strcpy(fetchAttr->requestMethod, m_httpMethod.c_str());

		fetchAttr->requestData = m_content.data();
		fetchAttr->requestDataSize = m_content.size();

		if (!m_requestHeaders.empty())
		{
			for (auto&& [header, value] : m_headers)
			{
				m_requestHeaders.push_back(header.c_str());
				m_requestHeaders.push_back(value.c_str());
			}
			m_requestHeaders.push_back(nullptr);

			fetchAttr->requestHeaders = m_requestHeaders.data();
		}

		m_fetchHandle = emscripten_fetch(fetchAttr, m_url.c_str());
		return m_fetchHandle;
	}
#endif
}
