// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderTarget::~RenderTarget()
	{
		OnRenderTargetRelease(this);
	}

	bool RenderTarget::IsActive() const
	{
		return Renderer::GetTarget() == this;
	}

	bool RenderTarget::SetActive(bool active)
	{
		if (active)
			return Renderer::SetTarget(this);
		else if (Renderer::GetTarget() == this)
			return Renderer::SetTarget(nullptr);

		return true;
	}

	void RenderTarget::Desactivate() const
	{
		// Seuls les target sans contextes (ex: RenderTexture) nécessitent une désactivation
	}
}
