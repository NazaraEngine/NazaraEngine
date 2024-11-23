// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/WebRequestResult.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/WebService.hpp>
#include <NazaraUtils/Algorithm.hpp>

#ifndef NAZARA_PLATFORM_WEB
#include <Nazara/Network/CurlLibrary.hpp>
#else
#include <emscripten/fetch.h>
#endif


namespace Nz
{
	UInt64 WebRequestResult::GetDownloadedSize() const
	{
		NazaraAssertMsg(HasSucceeded(), "web request failed");

#ifndef NAZARA_PLATFORM_WEB
		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t downloadedSize = 0;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_SIZE_DOWNLOAD_T, &downloadedSize);

		return SafeCast<UInt64>(downloadedSize);
#else
		return m_fetchHandle->numBytes;
#endif
	}

	UInt64 WebRequestResult::GetDownloadSpeed() const
	{
#ifndef NAZARA_PLATFORM_WEB
		NazaraAssertMsg(HasSucceeded(), "web request failed");

		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t downloadSpeed = 0;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);

		return SafeCast<UInt64>(downloadSpeed);
#else
		return 1000u * m_bodyResult.GetValue().size() / m_downloadTime.AsMilliseconds();
#endif
	}

	UInt32 WebRequestResult::GetStatusCode() const
	{
#ifndef NAZARA_PLATFORM_WEB
		assert(HasSucceeded());

		auto& libcurl = m_webService.GetCurlLibrary();

		long responseCode;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		return SafeCast<UInt32>(responseCode);
#else
		return m_fetchHandle->status;
#endif
	}
}
