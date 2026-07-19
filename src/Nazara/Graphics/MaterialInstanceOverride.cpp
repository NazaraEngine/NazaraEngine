// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/MaterialInstanceOverride.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderResourceReferences.hpp>
#include <NazaraUtils/Assert.hpp>

namespace Nz
{
	MaterialInstanceOverride::MaterialInstanceOverride(std::shared_ptr<const MaterialInstance> materialInstance) :
	m_materialInstance(std::move(materialInstance)),
	m_isShaderBindingInvalidated(true)
	{
		NazaraAssert(m_materialInstance);
		m_onMaterialInstanceShaderBindingInvalidated.Connect(m_materialInstance->OnMaterialInstanceShaderBindingInvalidated, [this](const MaterialInstance* /*matInstance*/)
		{
			m_isShaderBindingInvalidated = true;
		});

		m_bindingSetHashes.resize(m_materialInstance->GetBindingSetCount());
		for (UInt32 i = 0; i < m_bindingSetHashes.size(); ++i)
			m_bindingSetHashes[i] = m_materialInstance->GetBindingSetHash(i);
	}

	void MaterialInstanceOverride::FillRenderResourceReferences(RenderResourceReferences& resourceReferences) const
	{
		m_materialInstance->FillRenderResourceReferences(resourceReferences);

		for (const TextureOverride& textureOverride : m_textureOverride)
		{
			resourceReferences.samplers.insert(textureOverride.textureSampler);
			resourceReferences.textures.insert(textureOverride.texture);
		}
	}

	void MaterialInstanceOverride::FillSceneBindings(const ElementRenderer::SceneData& sceneData, std::vector<ShaderBinding::Binding>& bindings) const
	{
		return m_materialInstance->FillSceneBindings(sceneData, bindings);
	}

	void MaterialInstanceOverride::FillSkeletonBindings(const SkeletonInstance& skeleton, std::vector<ShaderBinding::Binding>& bindings) const
	{
		return m_materialInstance->FillSkeletonBindings(skeleton, bindings);
	}

	void MaterialInstanceOverride::FillViewerBindings(const AbstractViewer& viewer, std::vector<ShaderBinding::Binding>& bindings) const
	{
		return m_materialInstance->FillViewerBindings(viewer, bindings);
	}

	const ShaderBinding& MaterialInstanceOverride::GetShaderBinding(GpuResources& renderResources) const
	{
		if (m_shaderBinding && !m_isShaderBindingInvalidated)
			return *m_shaderBinding;

		if (m_shaderBinding)
			renderResources.PushForRelease(std::move(m_shaderBinding));

		const Material& material = *m_materialInstance->GetParentMaterial();

		m_shaderBinding = material.GetRenderPipelineLayout()->AllocateShaderBinding(Material::MaterialBindingSet);

		m_materialInstance->FillMaterialBindings([&](std::span<ShaderBinding::Binding> bindings)
		{
			for (const TextureOverride& textureOverride : m_textureOverride)
			{
				const auto& textureData = material.GetTextureData(textureOverride.textureIndex);

				auto it = std::find_if(bindings.begin(), bindings.end(), [&](ShaderBinding::Binding& binding)
				{
					return binding.bindingIndex == textureData.bindingIndex;
				});
				if (it != bindings.end())
				{
					it->content = ShaderBinding::SampledTextureBinding{
						textureOverride.texture.get(), textureOverride.textureSampler.get()
					};
				}
			}

			m_shaderBinding->Update(bindings.data(), bindings.size());
		});

		m_isShaderBindingInvalidated = false;

		return *m_shaderBinding;
	}

	void MaterialInstanceOverride::OverrideTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture, std::shared_ptr<TextureSampler> sampler)
	{
		if (!sampler)
			sampler = Graphics::Instance()->GetSamplerCache().Get({});

		auto it = std::find_if(m_textureOverride.begin(), m_textureOverride.end(), [&](const TextureOverride& textureOverride) { return textureOverride.textureIndex == textureIndex; });
		if (it != m_textureOverride.end())
		{
			it->texture = std::move(texture);
			it->textureSampler = std::move(sampler);
		}
		else
		{
			m_textureOverride.push_back({
				.textureIndex = textureIndex,
				.texture = std::move(texture),
				.textureSampler = std::move(sampler)
			});
		}

		m_isShaderBindingInvalidated = true;
	}

}
