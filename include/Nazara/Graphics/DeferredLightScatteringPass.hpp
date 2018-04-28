// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDLIGHTSCATTERINGPASS_HPP
#define NAZARA_DEFERREDLIGHTSCATTERINGPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Billboard.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API DeferredLightScatteringPass : public DeferredRenderPass
	{
		public:
			DeferredLightScatteringPass();
			virtual ~DeferredLightScatteringPass();

			unsigned int GetBlurPassCount() const;

			bool Process(const SceneData& sceneData, unsigned int firstWorkTexture, unsigned int secondWorkTexture) const override;
			bool Resize(const Vector2ui& GBufferSize) override;

			void SetBlurPassCount(unsigned int passCount);

		protected:
			Billboard m_test;
			mutable ForwardRenderTechnique m_renderTechnique;
			InstancedRenderable::InstanceData m_billboardInstanceData;
			InstancedRenderable::InstanceData m_billboardInstanceData2;
			RenderStates m_blendingStates;
			RenderStates m_states;
			RenderTexture m_occluderRTT;
			ShaderRef m_blendShader;
			ShaderRef m_gaussianBlurShader;
			ShaderRef m_occluderExtractionShader;
			ShaderRef m_lightScatteringShader;
			std::array<TextureRef, 2> m_occluderColorTextures;
			TextureRef m_occluderDepthTexture;
			TextureSampler m_bilinearSampler;
			TextureSampler m_pointSampler;
			int m_gaussianBlurShaderFilterLocation;
			int m_lightScatteringLightPositionLocation;
			unsigned int m_blurPassCount;
	};
}

#endif // NAZARA_DEFERREDLIGHTSCATTERINGPASS_HPP
