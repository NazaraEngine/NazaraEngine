// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredBloomPass.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzDeferredBloomPass::NzDeferredBloomPass() :
m_uniformUpdated(false),
m_brightLuminance(0.8),
m_brightMiddleGrey(0.5),
m_brightThreshold(0.8),
m_blurPassCount(5)
{
	m_bilinearSampler.SetAnisotropyLevel(1);
	m_bilinearSampler.SetFilterMode(nzSamplerFilter_Bilinear);
	m_bilinearSampler.SetWrapMode(nzSamplerWrap_Clamp);

	m_bloomBrightShader = NzShaderLibrary::Get("DeferredBloomBright");
	m_bloomFinalShader = NzShaderLibrary::Get("DeferredBloomFinal");
	m_bloomStates.parameters[nzRendererParameter_DepthBuffer] = false;
	m_gaussianBlurShader = NzShaderLibrary::Get("DeferredGaussianBlur");
	m_gaussianBlurShaderFilterLocation = m_gaussianBlurShader->GetUniformLocation("Filter");

	for (unsigned int i = 0; i < 2; ++i)
	{
		m_bloomTextures[i] = new NzTexture;
		m_bloomTextures[i]->SetPersistent(false);
	}
}

NzDeferredBloomPass::~NzDeferredBloomPass() = default;

unsigned int NzDeferredBloomPass::GetBlurPassCount() const
{
	return m_blurPassCount;
}

float NzDeferredBloomPass::GetBrightLuminance() const
{
	return m_brightLuminance;
}

float NzDeferredBloomPass::GetBrightMiddleGrey() const
{
	return m_brightMiddleGrey;
}

float NzDeferredBloomPass::GetBrightThreshold() const
{
	return m_brightThreshold;
}

NzTexture* NzDeferredBloomPass::GetTexture(unsigned int i) const
{
	#if NAZARA_GRAPHICS_SAFE
	if (i >= 2)
	{
		NazaraError("Texture index out of range (" + NzString::Number(i) + " >= 2)");
		return nullptr;
	}
	#endif

	return m_bloomTextures[i];
}

bool NzDeferredBloomPass::Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const
{
	NazaraUnused(scene);

	NzRenderer::SetRenderStates(m_bloomStates);
	NzRenderer::SetTextureSampler(0, m_bilinearSampler);
	NzRenderer::SetTextureSampler(1, m_bilinearSampler);

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShader(m_bloomBrightShader);
	if (!m_uniformUpdated)
	{
		m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightLuminance"), m_brightLuminance);
		m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightMiddleGrey"), m_brightMiddleGrey);
		m_bloomBrightShader->SendFloat(m_bloomBrightShader->GetUniformLocation("BrightThreshold"), m_brightThreshold);

		m_uniformUpdated = true;
	}

	NzRenderer::SetTexture(0, m_workTextures[secondWorkTexture]);
	NzRenderer::DrawFullscreenQuad();

	NzRenderer::SetTarget(&m_bloomRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x/8, m_dimensions.y/8));

	NzRenderer::SetShader(m_gaussianBlurShader);

	for (unsigned int i = 0; i < m_blurPassCount; ++i)
	{
		m_bloomRTT.SetColorTarget(0); // bloomTextureA

		m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, NzVector2f(1.f, 0.f));

		NzRenderer::SetTexture(0, (i == 0) ? m_workTextures[firstWorkTexture] : static_cast<const NzTexture*>(m_bloomTextures[1]));
		NzRenderer::DrawFullscreenQuad();

		m_bloomRTT.SetColorTarget(1); // bloomTextureB

		m_gaussianBlurShader->SendVector(m_gaussianBlurShaderFilterLocation, NzVector2f(0.f, 1.f));

		NzRenderer::SetTexture(0, m_bloomTextures[0]);
		NzRenderer::DrawFullscreenQuad();
	}

	m_workRTT->SetColorTarget(firstWorkTexture);
	NzRenderer::SetTarget(m_workRTT);
	NzRenderer::SetViewport(NzRecti(0, 0, m_dimensions.x, m_dimensions.y));

	NzRenderer::SetShader(m_bloomFinalShader);
	NzRenderer::SetTexture(0, m_bloomTextures[1]);
	NzRenderer::SetTexture(1, m_workTextures[secondWorkTexture]);
	NzRenderer::DrawFullscreenQuad();

	return true;
}

bool NzDeferredBloomPass::Resize(const NzVector2ui& dimensions)
{
	NzDeferredRenderPass::Resize(dimensions);

	m_bloomRTT.Create(true);
	for (unsigned int i = 0; i < 2; ++i)
	{
		m_bloomTextures[i]->Create(nzImageType_2D, nzPixelFormat_RGBA8, dimensions.x/8, dimensions.y/8);
		m_bloomRTT.AttachTexture(nzAttachmentPoint_Color, i, m_bloomTextures[i]);
	}
	m_bloomRTT.Unlock();

	if (!m_bloomRTT.IsComplete())
	{
		NazaraError("Incomplete RTT");
		return false;
	}

	return true;
}

void NzDeferredBloomPass::SetBlurPassCount(unsigned int passCount)
{
	m_blurPassCount = passCount; // N'est pas une uniforme
}

void NzDeferredBloomPass::SetBrightLuminance(float luminance)
{
	m_brightLuminance = luminance;
	m_uniformUpdated = false;
}

void NzDeferredBloomPass::SetBrightMiddleGrey(float middleGrey)
{
	m_brightMiddleGrey = middleGrey;
	m_uniformUpdated = false;
}

void NzDeferredBloomPass::SetBrightThreshold(float threshold)
{
	m_brightThreshold = threshold;
	m_uniformUpdated = false;
}
