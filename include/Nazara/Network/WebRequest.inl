// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void WebRequest::SetDataCallback(DataCallback callback)
	{
		m_dataCallback = std::move(callback);
	}

	inline void WebRequest::SetOptions(WebRequestOptionFlags options)
	{
		m_options = options;
	}

	inline void WebRequest::SetProgressCallback(ProgressCallback callback)
	{
		m_progressCallback = std::move(callback);
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

#ifdef NAZARA_PLATFORM_WEB
	inline emscripten_fetch_t* WebRequest::GetFetchHandle() const
	{
		return m_fetchHandle;
	}

	inline Time WebRequest::GetRequestTime() const
	{
		return m_clock.GetElapsedTime();
	}

	inline void WebRequest::StopClock()
	{
		m_clock.Pause();
	}
#endif

	inline void WebRequest::TriggerErrorCallback(std::string errorMessage)
	{
#ifndef NAZARA_PLATFORM_WEB
		m_resultCallback(WebRequestResult(m_webService, Nz::Err(std::move(errorMessage)), m_curlHandle.Get()));
#else
		m_resultCallback(WebRequestResult(m_webService, Nz::Err(std::move(errorMessage)), m_fetchHandle.Get(), m_clock.GetElapsedTime()));
#endif
	}

	inline void WebRequest::TriggerSuccessCallback()
	{
#ifndef NAZARA_PLATFORM_WEB
		m_resultCallback(WebRequestResult(m_webService, Nz::Ok(std::move(m_responseBody)), m_curlHandle.Get()));
#else
		m_resultCallback(WebRequestResult(m_webService, Nz::Ok(std::move(m_responseBody)), m_fetchHandle.Get(), m_clock.GetElapsedTime()));
#endif
		m_responseBody.clear();
	}
}
