// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBREQUESTRESULT_HPP
#define NAZARA_NETWORK_WEBREQUESTRESULT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Network/Config.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <functional>
#include <string>

using CURL = void;

namespace Nz
{
	class WebService;

	class NAZARA_NETWORK_API WebRequestResult
	{
		friend class WebRequest;

		public:
			WebRequestResult(const WebRequestResult&) = delete;
			WebRequestResult(WebRequestResult&&) = delete;
			~WebRequestResult() = default;

			inline std::string& GetBody();
			inline const std::string& GetBody() const;
			Nz::UInt64 GetDownloadedSize() const;
			Nz::UInt64 GetDownloadSpeed() const;
			inline const std::string& GetErrorMessage() const;
			long GetReponseCode() const;

			inline bool HasSucceeded() const;

			inline explicit operator bool() const;

			WebRequestResult& operator=(const WebRequestResult&) = delete;
			WebRequestResult& operator=(WebRequestResult&&) = delete;

		private:
			inline WebRequestResult(WebService& webService, CURL* curl, std::string body);
			inline WebRequestResult(WebService& webService, std::string errMessage);

			CURL* m_curlHandle;
			WebService& m_webService;
			std::string m_bodyOrErr;
	};
}

#include <Nazara/Network/WebRequestResult.inl>

#endif // NAZARA_NETWORK_WEBREQUESTRESULT_HPP
