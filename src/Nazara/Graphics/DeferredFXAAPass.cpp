// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFXAAPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DeferredFXAAPass::DeferredFXAAPass()
	{
		m_fxaaShader = ShaderLibrary::Get("DeferredFXAA");

		m_pointSampler.SetAnisotropyLevel(1);
		m_pointSampler.SetFilterMode(SamplerFilter_Nearest);
		m_pointSampler.SetWrapMode(SamplerWrap_Clamp);

		m_states.parameters[RendererParameter_DepthBuffer] = false;
	}

	DeferredFXAAPass::~DeferredFXAAPass() = default;

	bool DeferredFXAAPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
	{
		NazaraUnused(sceneData);

		m_workRTT->SetColorTarget(firstWorkTexture);
		Renderer::SetTarget(m_workRTT);
		Renderer::SetViewport(Recti(0, 0, m_dimensions.x, m_dimensions.y));

		Renderer::SetRenderStates(m_states);
		Renderer::SetShader(m_fxaaShader);
		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::SetTextureSampler(0, m_pointSampler);
		Renderer::DrawFullscreenQuad();

		return true;
	}
}
