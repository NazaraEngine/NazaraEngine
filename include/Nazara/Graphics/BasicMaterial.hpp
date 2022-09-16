// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BASICMATERIAL_HPP
#define NAZARA_GRAPHICS_BASICMATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/BasicMaterialPass.hpp>
#include <Nazara/Graphics/DepthMaterialPass.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <optional>

namespace Nz
{
	class Material;

	class NAZARA_GRAPHICS_API BasicMaterial
	{
		public:
			BasicMaterial(Material& material);
			BasicMaterial(const BasicMaterial&) = delete;
			BasicMaterial(BasicMaterial&&) = delete;
			~BasicMaterial() = default;

			inline void EnableAlphaTest(bool alphaTest);
			inline void EnableBlending(bool blending);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthClamp(bool depthClamp);
			inline void EnableDepthPass(bool depthPass);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableForwardPass(bool forwardPass);

			inline const std::shared_ptr<Texture>& GetAlphaMap() const;
			inline const TextureSamplerInfo& GetAlphaSampler() const;
			float GetAlphaTestThreshold() const;
			Color GetBaseColor() const;
			inline const std::shared_ptr<Texture>& GetBaseColorMap() const;
			inline const TextureSamplerInfo& GetBaseColorSampler() const;
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
			inline const nzsl::Ast::ConstantSingleValue& GetOptionValue(std::size_t optionIndex) const;
			inline const std::shared_ptr<MaterialPipeline>& GetPipeline() const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline PrimitiveMode GetPrimitiveMode() const;

			inline bool HasAlphaMap() const;
			inline bool HasAlphaTest() const;
			inline bool HasAlphaTestThreshold() const;
			inline bool HasBaseColor() const;
			inline bool HasBaseColorMap() const;

			inline bool IsAlphaTestEnabled() const;
			inline bool IsBlendingEnabled() const;
			inline bool IsColorWriteEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthClampEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsFlagEnabled(MaterialPassFlag flag) const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;

			inline void SetAlphaMap(std::shared_ptr<Texture> alphaMap);
			inline void SetAlphaSampler(TextureSamplerInfo alphaSampler);
			void SetAlphaTestThreshold(float alphaThreshold);
			void SetBaseColor(const Color& baseColor);
			inline void SetBaseColorMap(std::shared_ptr<Texture> baseColorMap);
			inline void SetBaseColorSampler(TextureSamplerInfo baseColorSampler);
			inline void SetBlendEquation(BlendEquation colorMode, BlendEquation alphaMode);
			inline void SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha);
			inline void SetDepthCompareFunc(RendererComparison depthFunc);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetPointSize(float pointSize);
			inline void SetPrimitiveMode(PrimitiveMode mode);

			BasicMaterial& operator=(const BasicMaterial&) = delete;
			BasicMaterial& operator=(BasicMaterial&&) = delete;

			static std::shared_ptr<Material> Build();

		protected:
			inline void UpdatePasses();

			std::optional<BasicMaterialPass> m_forwardPass;
			std::optional<DepthMaterialPass> m_depthPass;
			bool m_isDepthPassEnabled;
			bool m_isForwardPassEnabled;
	};
}

#include <Nazara/Graphics/BasicMaterial.inl>

#endif // NAZARA_GRAPHICS_BASICMATERIAL_HPP
