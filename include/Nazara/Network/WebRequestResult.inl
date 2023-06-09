// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline WebRequestResult::WebRequestResult(WebService& webService, CURL* curl, std::string body) :
	m_curlHandle(curl),
	m_webService(webService),
	m_bodyOrErr(std::move(body))
	{
	}

	inline WebRequestResult::WebRequestResult(WebService& webService, std::string errMessage) :
	m_curlHandle(nullptr),
	m_webService(webService),
	m_bodyOrErr(std::move(errMessage))
	{
	}

	inline std::string& WebRequestResult::GetBody()
	{
		assert(HasSucceeded());
		return m_bodyOrErr;
	}

	inline const std::string& WebRequestResult::GetBody() const
	{
		assert(HasSucceeded());
		return m_bodyOrErr;
	}

	inline const std::string& WebRequestResult::GetErrorMessage() const
	{
		assert(!HasSucceeded());
		return m_bodyOrErr;
	}

	inline bool WebRequestResult::HasSucceeded() const
	{
		return m_curlHandle != nullptr;
	}

	inline WebRequestResult::operator bool() const
	{
		return HasSucceeded();
	}

}

#include <Nazara/Network/DebugOff.hpp>
