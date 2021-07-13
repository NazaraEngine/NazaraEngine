// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_PASS_HPP
#define NAZARA_MATERIAL_PASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <array>
#include <string>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderFrame;

	class NAZARA_GRAPHICS_API MaterialPass : public Resource
	{
		public:
			MaterialPass(std::shared_ptr<const MaterialSettings> settings);
			inline ~MaterialPass();

			inline void Configure(std::shared_ptr<MaterialPipeline> pipeline);
			inline void Configure(const MaterialPipelineInfo& pipelineInfo);

			inline void EnableBlending(bool blending);
			inline void EnableColorWrite(bool colorWrite);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthClamp(bool depthClamp);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableOption(std::size_t optionIndex, bool enable);
			inline void EnableScissorTest(bool scissorTest);
			inline void EnableStencilTest(bool stencilTest);

			inline void EnsurePipelineUpdate() const;

			inline RendererComparison GetDepthCompareFunc() const;
			inline BlendEquation GetBlendAlphaModeEquation() const;
			inline BlendEquation GetBlendColorModeEquation() const;
			inline BlendFunc GetBlendDstAlphaFunc() const;
			inline BlendFunc GetBlendDstColorFunc() const;
			inline BlendFunc GetBlendSrcAlphaFunc() const;
			inline BlendFunc GetBlendSrcColorFunc() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline float GetLineWidth() const;
			inline const std::shared_ptr<MaterialPipeline>& GetPipeline() const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline PrimitiveMode GetPrimitiveMode() const;
			inline const std::shared_ptr<const MaterialSettings>& GetSettings() const;
			inline const std::shared_ptr<UberShader>& GetShader(ShaderStageType shaderStage) const;
			inline ShaderBinding& GetShaderBinding();
			inline const std::shared_ptr<Texture>& GetTexture(std::size_t textureIndex) const;
			inline const TextureSamplerInfo& GetTextureSampler(std::size_t textureIndex) const;
			inline const std::shared_ptr<AbstractBuffer>& GetUniformBuffer(std::size_t bufferIndex) const;
			inline const std::vector<UInt8>& GetUniformBufferConstData(std::size_t bufferIndex);
			inline std::vector<UInt8>& GetUniformBufferData(std::size_t bufferIndex);

			inline bool HasTexture(std::size_t textureIndex) const;

			inline bool IsBlendingEnabled() const;
			inline bool IsColorWriteEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthClampEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsOptionEnabled(std::size_t optionIndex) const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;

			inline void SetDepthCompareFunc(RendererComparison depthFunc);
			inline void SetBlendEquation(BlendEquation colorMode, BlendEquation alphaMode);
			inline void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetPointSize(float pointSize);
			inline void SetPrimitiveMode(PrimitiveMode mode);
			inline void SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			inline void SetTextureSampler(std::size_t textureIndex, TextureSamplerInfo samplerInfo);
			inline void SetUniformBuffer(std::size_t bufferIndex, std::shared_ptr<AbstractBuffer> uniformBuffer);

			bool Update(RenderFrame& renderFrame, CommandBufferBuilder& builder);

			// Signals:
			NazaraSignal(OnMaterialInvalidated, const MaterialPass* /*material*/);
			NazaraSignal(OnMaterialRelease, const MaterialPass* /*material*/);

		private:
			inline void InvalidatePipeline();
			inline void InvalidateShaderBinding();
			inline void InvalidateTextureSampler(std::size_t textureIndex);
			inline void InvalidateUniformData(std::size_t uniformBufferIndex);
			void UpdatePipeline() const;
			void UpdateShaderBinding();

			struct MaterialTexture
			{
				mutable std::shared_ptr<TextureSampler> sampler;
				std::shared_ptr<Texture> texture;
				TextureSamplerInfo samplerInfo;
			};

			struct UniformBuffer
			{
				std::shared_ptr<AbstractBuffer> buffer;
				std::vector<UInt8> data;
				bool dataInvalidated = true;
			};

			std::shared_ptr<const MaterialSettings> m_settings;
			std::vector<MaterialTexture> m_textures;
			std::vector<UniformBuffer> m_uniformBuffers;
			mutable std::shared_ptr<MaterialPipeline> m_pipeline;
			UInt64 m_enabledOptions;
			mutable MaterialPipelineInfo m_pipelineInfo;
			ShaderBindingPtr m_shaderBinding;
			mutable bool m_pipelineUpdated;
			bool m_shaderBindingUpdated;
	};
}

#include <Nazara/Graphics/MaterialPass.inl>

#endif // NAZARA_MATERIAL_PASS_HPP
