// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
#ifndef NAZARA_PLATFORM_WEB
	inline WebRequestResult::WebRequestResult(WebService& webService, Result<std::string, std::string>&& bodyResult, CURL* curl) :
	m_curlHandle(curl),
	m_webService(webService),
	m_bodyResult(std::move(bodyResult))
	{
	}
#else
	inline WebRequestResult::WebRequestResult(WebService& webService, Result<std::string, std::string>&& bodyResult, emscripten_fetch_t* fetchHandle, Time downloadTime) :
	m_fetchHandle(fetchHandle),
	m_webService(webService),
	m_bodyResult(std::move(bodyResult)),
	m_downloadTime(downloadTime)
	{
	}
#endif

	inline std::string& WebRequestResult::GetBody()
	{
		assert(HasSucceeded());
		return m_bodyResult.GetValue();
	}

	inline const std::string& WebRequestResult::GetBody() const
	{
		assert(HasSucceeded());
		return m_bodyResult.GetValue();
	}

	inline const std::string& WebRequestResult::GetErrorMessage() const
	{
		assert(!HasSucceeded());
		return m_bodyResult.GetError();
	}

	inline bool WebRequestResult::HasSucceeded() const
	{
		return m_bodyResult.IsOk();
	}

	inline WebRequestResult::operator bool() const
	{
		return HasSucceeded();
	}
}
