// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzRenderTarget::NzRenderTarget() :
m_listenersLocked(false)
{
}

NzRenderTarget::~NzRenderTarget()
{
	m_listenersLocked = true;
	for (auto& pair : m_listeners)
		pair.first->OnRenderTargetReleased(this, pair.second);
}

void NzRenderTarget::AddListener(Listener* listener, void* userdata) const
{
	NazaraError("What the");
	if (!m_listenersLocked)
		m_listeners.insert(std::make_pair(listener, userdata));
	NazaraError("What the");
}

bool NzRenderTarget::IsActive() const
{
	return NzRenderer::GetTarget() == this;
}

void NzRenderTarget::RemoveListener(Listener* listener) const
{
	if (!m_listenersLocked)
		m_listeners.erase(listener);
}

bool NzRenderTarget::SetActive(bool active)
{
	if (active)
		return NzRenderer::SetTarget(this);
	else if (NzRenderer::GetTarget() == this)
		return NzRenderer::SetTarget(nullptr);

	return true;
}

void NzRenderTarget::Desactivate() const
{
	// Seuls les target sans contextes (ex: NzRenderTexture) nécessitent une désactivation
}

void NzRenderTarget::NotifyParametersChange()
{
	m_listenersLocked = true;

	auto it = m_listeners.begin();
	while (it != m_listeners.end())
	{
		if (!it->first->OnRenderTargetParametersChange(this, it->second))
			m_listeners.erase(it++);
		else
			++it;
	}

	m_listenersLocked = false;
}

void NzRenderTarget::NotifySizeChange()
{
	m_listenersLocked = true;

	auto it = m_listeners.begin();
	while (it != m_listeners.end())
	{
		if (!it->first->OnRenderTargetSizeChange(this, it->second))
			m_listeners.erase(it++);
		else
			++it;
	}

	m_listenersLocked = false;
}

NzRenderTarget::Listener::~Listener() = default;

bool NzRenderTarget::Listener::OnRenderTargetParametersChange(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(renderTarget);
	NazaraUnused(userdata);

	return true;
}

void NzRenderTarget::Listener::OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(renderTarget);
	NazaraUnused(userdata);
}

bool NzRenderTarget::Listener::OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(renderTarget);
	NazaraUnused(userdata);

	return true;
}
