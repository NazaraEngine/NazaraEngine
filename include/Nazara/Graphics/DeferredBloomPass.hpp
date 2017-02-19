// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredBloomPass : public DeferredRenderPass
	{
		public:
			DeferredBloomPass();
			virtual ~DeferredBloomPass();

			unsigned int GetBlurPassCount() const;
			float GetBrightLuminance() const;
			float GetBrightMiddleGrey() const;
			float GetBrightThreshold() const;
			Texture* GetTexture(unsigned int i) const;

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const override;
			bool Resize(const Vector2ui& dimensions) override;

			void SetBlurPassCount(unsigned int passCount);
			void SetBrightLuminance(float luminance);
			void SetBrightMiddleGrey(float middleGrey);
			void SetBrightThreshold(float threshold);

		protected:
			RenderStates m_bloomStates;
			RenderTexture m_bloomRTT;
			ShaderRef m_bloomBrightShader;
			ShaderRef m_bloomFinalShader;
			ShaderRef m_gaussianBlurShader;
			TextureRef m_bloomTextures[2];
			TextureSampler m_bilinearSampler;
			mutable bool m_uniformUpdated;
			float m_brightLuminance;
			float m_brightMiddleGrey;
			float m_brightThreshold;
			int m_gaussianBlurShaderFilterLocation;
			unsigned int m_blurPassCount;
	};
}

#endif // NAZARA_DEFERREDBLOOMPASS_HPP
