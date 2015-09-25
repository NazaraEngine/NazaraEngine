// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredFinalPass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	DeferredFinalPass::DeferredFinalPass()
	{
		m_pointSampler.SetAnisotropyLevel(1);
		m_pointSampler.SetFilterMode(SamplerFilter_Nearest);
		m_pointSampler.SetWrapMode(SamplerWrap_Clamp);

		m_states.parameters[RendererParameter_DepthBuffer] = false;

		m_uberShader = UberShaderLibrary::Get("Basic");

		ParameterList list;
		list.SetParameter("AUTO_TEXCOORDS", true);
		list.SetParameter("DIFFUSE_MAPPING", true);
		list.SetParameter("TEXTURE_MAPPING", false);

		m_uberShaderInstance = m_uberShader->Get(list);

		const Shader* shader = m_uberShaderInstance->GetShader();
		m_materialDiffuseUniform = shader->GetUniformLocation("MaterialDiffuse");
		m_materialDiffuseMapUniform = shader->GetUniformLocation("MaterialDiffuseMap");
	}

	DeferredFinalPass::~DeferredFinalPass() = default;

	bool DeferredFinalPass::Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
	{
		NazaraAssert(sceneData.viewer, "Invalid viewer");

		NazaraUnused(firstWorkTexture);

		sceneData.viewer->ApplyView();

		Renderer::SetRenderStates(m_states);
		Renderer::SetTexture(0, m_workTextures[secondWorkTexture]);
		Renderer::SetTextureSampler(0, m_pointSampler);

		m_uberShaderInstance->Activate();

		const Shader* shader = m_uberShaderInstance->GetShader();
		shader->SendColor(m_materialDiffuseUniform, Color::White);
		shader->SendInteger(m_materialDiffuseMapUniform, 0);

		Renderer::DrawFullscreenQuad();

		return false;
	}
}
