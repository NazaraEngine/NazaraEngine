// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASS_HPP
#define NAZARA_GRAPHICS_MATERIALPASS_HPP

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
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <array>
#include <string>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderFrame;

	class NAZARA_GRAPHICS_API MaterialPass
	{
		public:
			MaterialPass(std::shared_ptr<const MaterialSettings> settings);
			MaterialPass(const MaterialPass&) = delete;
			MaterialPass(MaterialPass&&) = delete;
			inline ~MaterialPass();

			inline void Configure(std::shared_ptr<MaterialPipeline> pipeline);
			inline void Configure(const MaterialPipelineInfo& pipelineInfo);

			inline void EnableBlending(bool blending);
			inline void EnableColorWrite(bool colorWrite);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthClamp(bool depthClamp);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableFlag(MaterialPassFlag flag, bool enable = true);
			inline void EnableScissorTest(bool scissorTest);
			inline void EnableStencilTest(bool stencilTest);

			inline void EnsurePipelineUpdate() const;

			void FillShaderBinding(std::vector<ShaderBinding::Binding>& bindings) const;

			inline RendererComparison GetDepthCompareFunc() const;
			inline BlendEquation GetBlendAlphaModeEquation() const;
			inline BlendEquation GetBlendColorModeEquation() const;
			inline BlendFunc GetBlendDstAlphaFunc() const;
			inline BlendFunc GetBlendDstColorFunc() const;
			inline BlendFunc GetBlendSrcAlphaFunc() const;
			inline BlendFunc GetBlendSrcColorFunc() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline MaterialPassFlags GetFlags() const;
			inline float GetLineWidth() const;
			inline const ShaderAst::ConstantValue& GetOptionValue(std::size_t optionIndex) const;
			inline const std::shared_ptr<MaterialPipeline>& GetPipeline() const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline PrimitiveMode GetPrimitiveMode() const;
			inline const std::shared_ptr<const MaterialSettings>& GetSettings() const;
			inline const std::shared_ptr<UberShader>& GetShader(ShaderStageType shaderStage) const;
			inline const std::shared_ptr<Texture>& GetTexture(std::size_t textureIndex) const;
			inline const TextureSamplerInfo& GetTextureSampler(std::size_t textureIndex) const;
			inline const std::shared_ptr<RenderBuffer>& GetUniformBuffer(std::size_t bufferIndex) const;
			inline const std::vector<UInt8>& GetUniformBufferConstData(std::size_t bufferIndex) const;
			inline std::vector<UInt8>& GetUniformBufferData(std::size_t bufferIndex);

			inline bool HasTexture(std::size_t textureIndex) const;

			inline bool IsBlendingEnabled() const;
			inline bool IsColorWriteEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthClampEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsFlagEnabled(MaterialPassFlag flag) const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;

			inline void SetDepthCompareFunc(RendererComparison depthFunc);
			inline void SetBlendEquation(BlendEquation colorMode, BlendEquation alphaMode);
			inline void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetOptionValue(std::size_t optionIndex, ShaderAst::ConstantValue value);
			inline void SetPointSize(float pointSize);
			inline void SetPrimitiveMode(PrimitiveMode mode);
			inline void SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			inline void SetTextureSampler(std::size_t textureIndex, TextureSamplerInfo samplerInfo);
			inline void SetUniformBuffer(std::size_t bufferIndex, std::shared_ptr<RenderBuffer> uniformBuffer);

			void Update(RenderFrame& renderFrame, CommandBufferBuilder& builder);

			MaterialPass& operator=(const MaterialPass&) = delete;
			MaterialPass& operator=(MaterialPass&&) = delete;

			// Signals:
			NazaraSignal(OnMaterialPassInvalidated, const MaterialPass* /*materialPass*/);
			NazaraSignal(OnMaterialPassPipelineInvalidated, const MaterialPass* /*materialPass*/);
			NazaraSignal(OnMaterialPassShaderBindingInvalidated, const MaterialPass* /*materialPass*/);
			NazaraSignal(OnMaterialPassRelease, const MaterialPass* /*materialPass*/);

		private:
			inline void InvalidatePipeline();
			inline void InvalidateShaderBinding();
			inline void InvalidateTextureSampler(std::size_t textureIndex);
			inline void InvalidateUniformData(std::size_t uniformBufferIndex);
			void UpdatePipeline() const;

			struct MaterialTexture
			{
				mutable std::shared_ptr<TextureSampler> sampler;
				std::shared_ptr<Texture> texture;
				TextureSamplerInfo samplerInfo;
			};

			struct ShaderEntry
			{
				NazaraSlot(UberShader, OnShaderUpdated, onShaderUpdated);
			};

			struct UniformBuffer
			{
				std::shared_ptr<RenderBuffer> buffer;
				std::vector<UInt8> data;
				bool dataInvalidated = true;
			};

			std::array<ShaderAst::ConstantValue, 64> m_optionValues;
			std::shared_ptr<const MaterialSettings> m_settings;
			std::vector<MaterialTexture> m_textures;
			std::vector<ShaderEntry> m_shaders;
			std::vector<UniformBuffer> m_uniformBuffers;
			mutable std::shared_ptr<MaterialPipeline> m_pipeline;
			mutable MaterialPipelineInfo m_pipelineInfo;
			MaterialPassFlags m_flags;
			mutable bool m_pipelineUpdated;
	};
}

#include <Nazara/Graphics/MaterialPass.inl>

#endif // NAZARA_GRAPHICS_MATERIALPASS_HPP
