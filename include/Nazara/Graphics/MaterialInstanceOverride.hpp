// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALINSTANCEOVERRIDE_HPP
#define NAZARA_GRAPHICS_MATERIALINSTANCEOVERRIDE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <NazaraUtils/FixedVector.hpp>

namespace Nz
{
	class Texture;
	class TextureSampler;

	class NAZARA_GRAPHICS_API MaterialInstanceOverride final : public MaterialProxy
	{
		public:
			MaterialInstanceOverride(std::shared_ptr<const MaterialInstance> materialInstance);
			MaterialInstanceOverride(const MaterialInstanceOverride&) = delete;
			MaterialInstanceOverride(MaterialInstanceOverride&&) = delete;
			~MaterialInstanceOverride() = default;

			void FillRenderResourceReferences(RenderResourceReferences& resourceReferences) const override;
			void FillSceneBindings(const ElementRenderer::SceneData& sceneData, std::vector<ShaderBinding::Binding>& bindings) const override;
			void FillSkeletonBindings(const SkeletonInstance& skeleton, std::vector<ShaderBinding::Binding>& bindings) const override;
			void FillViewerBindings(const AbstractViewer& viewer, std::vector<ShaderBinding::Binding>& bindings) const override;

			const ShaderBinding& GetShaderBinding(RenderResources& renderResources) const override;

			void OverrideTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture, std::shared_ptr<TextureSampler> sampler = nullptr);

			MaterialInstanceOverride& operator=(const MaterialInstanceOverride&) = delete;
			MaterialInstanceOverride& operator=(MaterialInstanceOverride&&) = delete;

		private:
			struct TextureOverride
			{
				std::size_t textureIndex;
				std::shared_ptr<Texture> texture;
				std::shared_ptr<TextureSampler> textureSampler;
			};

			NazaraSlot(MaterialInstance, OnMaterialInstanceShaderBindingInvalidated, m_onMaterialInstanceShaderBindingInvalidated);

			std::shared_ptr<const MaterialInstance> m_materialInstance;
			HybridVector<TextureOverride, 1> m_textureOverride;
			mutable ShaderBindingPtr m_shaderBinding;
			mutable bool m_isShaderBindingInvalidated;
	};
}

#include <Nazara/Graphics/MaterialInstanceOverride.inl>

#endif // NAZARA_GRAPHICS_MATERIALINSTANCEOVERRIDE_HPP
