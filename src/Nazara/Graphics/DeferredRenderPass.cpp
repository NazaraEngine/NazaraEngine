// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredRenderPass::NzDeferredRenderPass() :
m_enabled(true)
{
}

NzDeferredRenderPass::~NzDeferredRenderPass() = default;

void NzDeferredRenderPass::Enable(bool enable)
{
	m_enabled = enable;
}

void NzDeferredRenderPass::Initialize(NzDeferredRenderTechnique* technique)
{
	m_deferredTechnique = technique;
	m_renderQueue = static_cast<NzDeferredRenderQueue*>(technique->GetRenderQueue());

	m_depthStencilBuffer = technique->GetDepthStencilBuffer();

	m_GBufferRTT = technique->GetGBufferRTT();
	for (unsigned int i = 0; i < 3; ++i)
		m_GBuffer[i] = technique->GetGBuffer(i);

	m_workRTT = technique->GetWorkRTT();
	for (unsigned int i = 0; i < 2; ++i)
		m_workTextures[i] = technique->GetWorkTexture(i);
}

bool NzDeferredRenderPass::IsEnabled() const
{
	return m_enabled;
}

bool NzDeferredRenderPass::Resize(const NzVector2ui& dimensions)
{
	m_dimensions = dimensions;

	return true;
}
