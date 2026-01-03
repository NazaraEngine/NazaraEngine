// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline std::unique_ptr<WebRequest> WebService::AllocateRequest()
	{
		return std::make_unique<WebRequest>(*this);
	}

	inline std::unique_ptr<WebRequest> WebService::CreateRequest(WebRequestMethod method, const std::string& url, WebRequest::ResultCallback callback)
	{
		std::unique_ptr<WebRequest> request = AllocateRequest();
		request->SetMethod(method);
		request->SetURL(url);
		request->SetResultCallback(std::move(callback));

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
