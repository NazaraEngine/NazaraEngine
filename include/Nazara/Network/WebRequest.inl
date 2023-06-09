// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline void WebRequest::SetDataCallback(DataCallback callback)
	{
		m_dataCallback = std::move(callback);
	}

	inline void WebRequest::SetResultCallback(ResultCallback callback)
	{
		m_resultCallback = std::move(callback);
	}

	inline void WebRequest::SetHeader(std::string header, std::string value)
	{
		m_headers.insert_or_assign(std::move(header), std::move(value));
	}
	
	inline bool WebRequest::OnBodyResponse(const char* data, std::size_t length)
	{
		if (!m_dataCallback)
		{
			m_responseBody.append(data, length);
			return true;
		}

		return m_dataCallback(data, length);
	}

	inline void WebRequest::TriggerCallback()
	{
		m_resultCallback(WebRequestResult(m_webService, m_curlHandle.Get(), std::move(m_responseBody)));
		m_responseBody.clear();
	}

	inline void WebRequest::TriggerCallback(std::string errorMessage)
	{
		m_resultCallback(WebRequestResult(m_webService, std::move(errorMessage)));
	}
}

#include <Nazara/Network/DebugOff.hpp>
