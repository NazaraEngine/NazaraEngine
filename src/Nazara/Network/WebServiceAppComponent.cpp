// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/WebServiceAppComponent.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Network/WebService.hpp>

namespace Nz
{
	WebServiceAppComponent::WebServiceAppComponent(ApplicationBase& app) :
	ApplicationComponent(app)
	{
		m_webService = Network::Instance()->InstantiateWebService();
	}

	std::unique_ptr<WebRequest> WebServiceAppComponent::AllocateRequest()
	{
		return m_webService->AllocateRequest();
	}

	void WebServiceAppComponent::QueueRequest(const FunctionRef<bool(WebRequest& request)>& builder)
	{
		m_webService->QueueRequest(builder);
	}

	void WebServiceAppComponent::QueueRequest(std::unique_ptr<WebRequest>&& request)
	{
		m_webService->QueueRequest(std::move(request));
	}

	void WebServiceAppComponent::Update(Time /*elapsedTime*/)
	{
		m_webService->Poll();
	}
}
