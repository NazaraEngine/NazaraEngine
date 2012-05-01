// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzRenderTarget::~NzRenderTarget() = default;

bool NzRenderTarget::IsActive() const
{
	return NazaraRenderer->GetTarget() == this;
}

bool NzRenderTarget::SetActive(bool active)
{
	if (active)
		return NazaraRenderer->SetTarget(this);
	else if (NazaraRenderer->GetTarget() == this)
		return NazaraRenderer->SetTarget(nullptr);

	return true;
}

void NzRenderTarget::Desactivate()
{
	// Seuls les target sans contextes (ex: RenderTexture) nécessitent une désactivation
}
