// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/WebService.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#ifndef NAZARA_PLATFORM_WEB
#include <Nazara/Network/CurlLibrary.hpp>
#else
#include <emscripten/fetch.h>
#endif
#include <fmt/format.h>

namespace Nz
{
#ifndef NAZARA_PLATFORM_WEB
	WebService::WebService(const CurlLibrary& curl) :
	m_curl(curl)
	{
		curl_version_info_data* curlVersionData = m_curl.version_info(CURLVERSION_NOW);

		m_userAgent = fmt::format("Nazara WebService - curl/{}", curlVersionData->version);

		m_curlMulti = m_curl.multi_init();
	}
#else
	WebService::WebService() :
	m_userAgent("Nazara WebService - emscripten_fetch")
	{
	}
#endif

	WebService::~WebService()
	{
#ifndef NAZARA_PLATFORM_WEB
		if (m_curlMulti)
		{
			for (auto&& [handle, request] : m_activeRequests)
				m_curl.multi_remove_handle(m_curlMulti, handle);

			m_curl.multi_cleanup(m_curlMulti);
		}
#endif
	}

	bool WebService::Poll()
	{
#ifndef NAZARA_PLATFORM_WEB
		assert(m_curlMulti);

		int reportedActiveRequest;
		CURLMcode err = m_curl.multi_perform(m_curlMulti, &reportedActiveRequest);
		if (err != CURLM_OK)
		{
			NazaraError(fmt::format("[WebService] curl_multi_perform failed with {0}: {1}", UnderlyingCast(err), m_curl.multi_strerror(err)));
			return false;
		}

		bool finishedRequest = false;

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
					request.TriggerSuccessCallback();
				else
					request.TriggerErrorCallback(m_curl.easy_strerror(m->data.result));

				m_curl.multi_remove_handle(m_curlMulti, handle);

				m_activeRequests.erase(handle);

				finishedRequest = true;
			}
		}
		while (m);

		return finishedRequest; //< returns true if at least one request finished
#else
		if (m_finishedRequests.empty())
			return false;

		for (auto&& [request, succeeded] : m_finishedRequests)
		{
			if (succeeded)
				request->TriggerSuccessCallback();
			else
				request->TriggerErrorCallback(request->GetFetchHandle()->statusText);
		}

		m_finishedRequests.clear();
		return true;
#endif
	}

	void WebService::QueueRequest(const FunctionRef<bool(WebRequest& request)>& builder)
	{
		std::unique_ptr<WebRequest> request = AllocateRequest();
		if (!builder(*request))
			return;

		QueueRequest(std::move(request));
	}

	void WebService::QueueRequest(std::unique_ptr<WebRequest>&& request)
	{
		assert(request);

#ifndef NAZARA_PLATFORM_WEB
		assert(m_curlMulti);

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
#else
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);

		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

		attr.onsuccess = [](emscripten_fetch_t* fetch)
		{
			WebService* service = static_cast<WebService*>(fetch->userData);

			auto it = service->m_activeRequests.find(fetch);
			if (it == service->m_activeRequests.end())
			{
				NazaraError("received emscripten fetch onsuccess with unbound request");
				return;
			}

			std::unique_ptr<WebRequest>& request = it->second;
			request->StopClock();
			request->OnBodyResponse(fetch->data, SafeCast<std::size_t>(fetch->numBytes));

			service->m_finishedRequests.push_back({
				std::move(request),
				true
			});
		};

		attr.onerror = [](emscripten_fetch_t* fetch)
		{
			WebService* service = static_cast<WebService*>(fetch->userData);

			auto it = service->m_activeRequests.find(fetch);
			if (it == service->m_activeRequests.end())
			{
				NazaraError("received emscripten fetch onsuccess with unbound request");
				return;
			}

			std::unique_ptr<WebRequest>& request = it->second;
			request->StopClock();

			service->m_finishedRequests.push_back({
				std::move(request),
				false
			});
		};

		attr.userData = this;

		emscripten_fetch_t* handle = request->Prepare(&attr);
		m_activeRequests.emplace(handle, std::move(request));
#endif
	}
}
