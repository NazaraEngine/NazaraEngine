// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DeferredRenderPass::DeferredRenderPass() :
	m_enabled(true)
	{
	}

	DeferredRenderPass::~DeferredRenderPass() = default;

	void DeferredRenderPass::Enable(bool enable)
	{
		m_enabled = enable;
	}

	void DeferredRenderPass::Initialize(DeferredRenderTechnique* technique)
	{
		m_deferredTechnique = technique;
		m_renderQueue = static_cast<DeferredRenderQueue*>(technique->GetRenderQueue());

		m_depthStencilBuffer = technique->GetDepthStencilBuffer();

		m_GBufferRTT = technique->GetGBufferRTT();
		for (unsigned int i = 0; i < 3; ++i)
			m_GBuffer[i] = technique->GetGBuffer(i);

		m_workRTT = technique->GetWorkRTT();
		for (unsigned int i = 0; i < 2; ++i)
			m_workTextures[i] = technique->GetWorkTexture(i);
	}

	bool DeferredRenderPass::IsEnabled() const
	{
		return m_enabled;
	}

	bool DeferredRenderPass::Resize(const Vector2ui& dimensions)
	{
		m_dimensions = dimensions;

		return true;
	}
}
