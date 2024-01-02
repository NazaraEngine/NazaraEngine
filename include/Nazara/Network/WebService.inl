// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline std::unique_ptr<WebRequest> WebService::AllocateRequest()
	{
		return std::make_unique<WebRequest>(*this);
	}

	inline std::unique_ptr<WebRequest> WebService::CreateGetRequest(const std::string& url, WebRequest::ResultCallback callback)
	{
		std::unique_ptr<WebRequest> request = AllocateRequest();
		request->SetURL(url);
		request->SetResultCallback(std::move(callback));
		request->SetupGet();

		return request;
	}

	inline std::unique_ptr<WebRequest> WebService::CreatePostRequest(const std::string& url, WebRequest::ResultCallback callback)
	{
		std::unique_ptr<WebRequest> request = AllocateRequest();
		request->SetURL(url);
		request->SetResultCallback(std::move(callback));
		request->SetupPost();

		return request;
	}

	inline const std::string& WebService::GetUserAgent() const
	{
		return m_userAgent;
	}

#ifndef NAZARA_PLATFORM_WEB
	const CurlLibrary& WebService::GetCurlLibrary() const
	{
		return m_curl;
	}
#endif
}

#include <Nazara/Network/DebugOff.hpp>
