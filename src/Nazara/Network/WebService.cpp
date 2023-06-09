// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/WebService.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Network/CurlLibrary.hpp> //< include last because of curl/curl.h
#include <fmt/format.h>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	WebService::WebService(const CurlLibrary& curl) :
	m_curl(curl)
	{
		curl_version_info_data* curlVersionData = m_curl.version_info(CURLVERSION_NOW);

		m_userAgent = fmt::format("Nazara WebService - curl/{}", curlVersionData->version);

		m_curlMulti = m_curl.multi_init();
	}

	WebService::~WebService()
	{
		if (m_curlMulti)
		{
			for (auto&& [handle, request] : m_activeRequests)
				m_curl.multi_remove_handle(m_curlMulti, handle);

			m_curl.multi_cleanup(m_curlMulti);
		}
	}
	
	void WebService::Poll()
	{
		assert(m_curlMulti);

		int reportedActiveRequest;
		CURLMcode err = m_curl.multi_perform(m_curlMulti, &reportedActiveRequest);
		if (err != CURLM_OK)
		{
			NazaraError(fmt::format("[WebService] curl_multi_perform failed with {0}: {1}", UnderlyingCast(err), m_curl.multi_strerror(err)));
			return;
		}

		CURLMsg* m;
		do
		{
			int msgq;
			m = m_curl.multi_info_read(m_curlMulti, &msgq);
			if (m && (m->msg == CURLMSG_DONE))
			{
				CURL* handle = m->easy_handle;

				auto it = m_activeRequests.find(handle);
				assert(it != m_activeRequests.end());

				WebRequest& request = *it->second;

				if (m->data.result == CURLE_OK)
					request.TriggerCallback();
				else
					request.TriggerCallback(m_curl.easy_strerror(m->data.result));

				m_curl.multi_remove_handle(m_curlMulti, handle);

				m_activeRequests.erase(handle);
			}
		}
		while (m);
	}

	void WebService::QueueRequest(std::unique_ptr<WebRequest>&& request)
	{
		assert(m_curlMulti);
		assert(request);

		CURL* handle = request->Prepare();

		curl_write_callback writeCallback = [](char* ptr, std::size_t size, std::size_t nmemb, void* userdata) -> std::size_t
		{
			WebRequest* request = static_cast<WebRequest*>(userdata);

			std::size_t totalSize = size * nmemb;
			if (!request->OnBodyResponse(ptr, totalSize))
				return 0;

			return totalSize;
		};

		m_curl.easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback);
		m_curl.easy_setopt(handle, CURLOPT_WRITEDATA, request.get());

		m_activeRequests.emplace(handle, std::move(request));

		m_curl.multi_add_handle(m_curlMulti, handle);
	}
}
