// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPASS_HPP
#define NAZARA_GRAPHICS_MATERIALPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <NZSL/Ast/ConstantValue.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace Nz
{
	struct MaterialPass
	{
		MaterialPassFlags flags;
		RenderStates states;
		std::unordered_map<UInt32, nzsl::Ast::ConstantSingleValue> options;
		std::vector<std::shared_ptr<UberShader>> shaders;
	};
#if 0
	class CommandBufferBuilder;
	class RenderFrame;

	class NAZARA_GRAPHICS_API MaterialPass
	{
		public:
			struct Settings;

			MaterialPass(Settings&& settings);
			MaterialPass(const MaterialPass&) = delete;
			MaterialPass(MaterialPass&&) = delete;
			inline ~MaterialPass();

			inline void Configure(const RenderStates& renderStates);

			inline void EnableBlending(bool blending);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthClamp(bool depthClamp);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableFlag(MaterialPassFlag flag, bool enable = true);
			inline void EnableScissorTest(bool scissorTest);
			inline void EnableStencilTest(bool stencilTest);

			inline BlendEquation GetBlendAlphaModeEquation() const;
			inline BlendEquation GetBlendColorModeEquation() const;
			inline BlendFunc GetBlendDstAlphaFunc() const;
			inline BlendFunc GetBlendDstColorFunc() const;
			inline BlendFunc GetBlendSrcAlphaFunc() const;
			inline BlendFunc GetBlendSrcColorFunc() const;
			inline ColorComponentMask GetColorWriteMask() const;
			inline RendererComparison GetDepthCompareFunc() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline MaterialPassFlags GetFlags() const;
			inline float GetLineWidth() const;
			inline const nzsl::Ast::ConstantSingleValue* GetOptionValue(UInt32 optionHash) const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline PrimitiveMode GetPrimitiveMode() const;
			inline const std::shared_ptr<UberShader>& GetShader(nzsl::ShaderStageType shaderStage) const;

			inline bool IsBlendingEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthClampEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsFlagEnabled(MaterialPassFlag flag) const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;

			inline void SetBlendEquation(BlendEquation colorMode, BlendEquation alphaMode);
			inline void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha);
			inline void SetColorWriteMask(ColorComponentMask colorMask);
			inline void SetDepthCompareFunc(RendererComparison depthFunc);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetOptionValue(UInt32 optionHash, const nzsl::Ast::ConstantSingleValue& value);
			inline void SetPointSize(float pointSize);
			inline void SetPrimitiveMode(PrimitiveMode mode);

			MaterialPass& operator=(const MaterialPass&) = delete;
			MaterialPass& operator=(MaterialPass&&) = delete;

			// Signals:
			NazaraSignal(OnMaterialPassPipelineInvalidated, const MaterialPass* /*materialPass*/);
			NazaraSignal(OnMaterialPassRelease, const MaterialPass* /*materialPass*/);

			struct Settings
			{
				struct ShaderInfo
				{
					std::shared_ptr<UberShader> uberShader;
				};

				std::shared_ptr<RenderPipelineLayout> pipelineLayout;
				std::vector<ShaderInfo> shaders;
			};

		private:
			inline void InvalidatePipeline();
			void UpdatePipeline() const;

			struct ShaderEntry
			{
				NazaraSlot(UberShader, OnShaderUpdated, onShaderUpdated);
			};

			std::vector<ShaderEntry> m_shaders;
			mutable MaterialPipelineInfo m_pipelineInfo;
			MaterialPassFlags m_flags;
	};
#endif
}

#include <Nazara/Graphics/MaterialPass.inl>

#endif // NAZARA_GRAPHICS_MATERIALPASS_HPP
