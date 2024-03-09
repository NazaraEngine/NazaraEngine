// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_WEBSERVICEAPPCOMPONENT_HPP
#define NAZARA_NETWORK_WEBSERVICEAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Network/Export.hpp>
#include <Nazara/Network/WebRequest.hpp>
#include <NazaraUtils/FunctionRef.hpp>

namespace Nz
{
	class WebService;

	class NAZARA_NETWORK_API WebServiceAppComponent final : public ApplicationComponent
	{
		public:
			WebServiceAppComponent(ApplicationBase& app);
			WebServiceAppComponent(const WebServiceAppComponent&) = delete;
			WebServiceAppComponent(WebServiceAppComponent&&) = delete;
			~WebServiceAppComponent() = default;

			std::unique_ptr<WebRequest> AllocateRequest();

			void QueueRequest(const FunctionRef<bool(WebRequest& request)>& builder);
			void QueueRequest(std::unique_ptr<WebRequest>&& request);

			WebServiceAppComponent& operator=(const WebServiceAppComponent&) = delete;
			WebServiceAppComponent& operator=(WebServiceAppComponent&&) = delete;

		private:
			void Update(Time elapsedTime) override;

			std::shared_ptr<WebService> m_webService;
	};
}

#include <Nazara/Network/WebServiceAppComponent.inl>

#endif // NAZARA_NETWORK_WEBSERVICEAPPCOMPONENT_HPP
