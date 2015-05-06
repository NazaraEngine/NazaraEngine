// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzRenderTarget::~NzRenderTarget()
{
	NotifyRelease(Listener::OnRenderTargetReleased);
}

bool NzRenderTarget::IsActive() const
{
	return NzRenderer::GetTarget() == this;
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
