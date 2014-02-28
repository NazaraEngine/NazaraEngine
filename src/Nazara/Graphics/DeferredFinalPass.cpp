// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFinalPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderLibrary.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredFinalPass::NzDeferredFinalPass()
{
	m_pointSampler.SetAnisotropyLevel(1);
	m_pointSampler.SetFilterMode(nzSamplerFilter_Nearest);
	m_pointSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_states.parameters[nzRendererParameter_DepthBuffer] = false;

	m_uberShader = NzUberShaderLibrary::Get("Basic");

	NzParameterList list;
	list.SetParameter("AUTO_TEXCOORDS", true);
	list.SetParameter("DIFFUSE_MAPPING", true);
	list.SetParameter("TEXTURE_MAPPING", false);

	m_uberShaderInstance = m_uberShader->Get(list);

	const NzShader* shader = m_uberShaderInstance->GetShader();
	m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
	m_materialDiffuseMapUniform = shader->GetUniformLocation("MaterialDiffuseMap");
}

NzDeferredFinalPass::~NzDeferredFinalPass() = default;

bool NzDeferredFinalPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(firstWorkTexture);

	scene->GetViewer()->ApplyView();

	NzRenderer::SetRenderStates(m_states);
	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::SetTextureSampler(0, m_pointSampler);

	m_uberShaderInstance->Activate();

	const NzShader* shader = m_uberShaderInstance->GetShader();
	shader->SendColor(m_materialDiffuseUniform, NzColor::White);
	shader->SendInteger(m_materialDiffuseMapUniform, 0);

	NzRenderer::DrawFullscreenQuad();

	return false;
}
