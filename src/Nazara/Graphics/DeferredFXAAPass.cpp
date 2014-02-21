// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFXAAPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredFXAAPass::NzDeferredFXAAPass()
{
	m_fxaaShader = NzShaderLibrary::Get("DeferredFXAA");

	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;
}

NzDeferredFXAAPass::~NzDeferredFXAAPass() = default;

bool NzDeferredFXAAPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(scene);

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetRenderStates(m_states);
	NzRenderer::SetShader(m_fxaaShader);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);
	NzRenderer::DrawFullscreenQuad();

	return true;
}
