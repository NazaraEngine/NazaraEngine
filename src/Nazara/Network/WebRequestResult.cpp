// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/WebRequestResult.hpp>
#include <Nazara/Network/CurlLibrary.hpp> //< include last because of curl/curl.h
#include <Nazara/Network/WebService.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	Nz::UInt64 WebRequestResult::GetDownloadedSize() const
	{
		assert(HasSucceeded());

		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t downloadedSize = 0;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_SIZE_DOWNLOAD_T, &downloadedSize);

		return downloadedSize;
	}

	std::size_t WebRequestResult::GetDownloadSpeed() const
	{
		assert(HasSucceeded());

		auto& libcurl = m_webService.GetCurlLibrary();

		curl_off_t downloadSpeed = 0;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);

		return downloadSpeed;
	}

	long WebRequestResult::GetReponseCode() const
	{
		assert(HasSucceeded());

		auto& libcurl = m_webService.GetCurlLibrary();

		long responseCode;
		libcurl.easy_getinfo(m_curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		return responseCode;
	}
}
