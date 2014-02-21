// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDBLOOMPASS_HPP
#define NAZARA_DEFERREDBLOOMPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

class NAZARA_API NzDeferredBloomPass : public NzDeferredRenderPass
{
	public:
		NzDeferredBloomPass();
		virtual ~NzDeferredBloomPass();

		unsigned int GetBlurPassCount() const;
		float GetBrightLuminance() const;
		float GetBrightMiddleGrey() const;
		float GetBrightThreshold() const;
		NzTexture* GetTexture(unsigned int i) const;

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;
		bool Resize(const NzVector2ui& dimensions);

		void SetBlurPassCount(unsigned int passCount);
		void SetBrightLuminance(float luminance);
		void SetBrightMiddleGrey(float middleGrey);
		void SetBrightThreshold(float threshold);

	protected:
		NzRenderStates m_bloomStates;
		NzRenderTexture m_bloomRTT;
		NzShaderRef m_bloomBrightShader;
		NzShaderRef m_bloomFinalShader;
		NzShaderRef m_gaussianBlurShader;
		NzTextureRef m_bloomTextures[2];
		NzTextureSampler m_bilinearSampler;
		mutable bool m_uniformUpdated;
		float m_brightLuminance;
		float m_brightMiddleGrey;
		float m_brightThreshold;
		int m_gaussianBlurShaderFilterLocation;
		unsigned int m_blurPassCount;
};

#endif // NAZARA_DEFERREDBLOOMPASS_HPP
