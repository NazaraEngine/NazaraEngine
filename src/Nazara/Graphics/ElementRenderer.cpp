// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const RenderData& /*renderData*/, const SceneData& /*sceneData*/, const AbstractViewer& /*viewer*/, ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, std::size_t /*elementCount*/, const Pointer<const RenderElement>* /*elements*/)
	{
	}

	void ElementRenderer::PrepareEnd(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, CommandBufferBuilder& /*commandBuffer*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/)
	{
	}

	void ElementRenderer::FillSceneBindings(const SceneData& sceneData, const Material& material, std::vector<ShaderBinding::Binding>& bindings)
	{
		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalLights); bindingIndex != Material::InvalidBindingIndex && sceneData.directionalLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.directionalLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.directionalLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.directionalLightAtlasMapping);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointLights); bindingIndex != Material::InvalidBindingIndex && sceneData.pointLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.pointLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.pointLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.pointLightAtlasMapping);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowAtlas); bindingIndex != Material::InvalidBindingIndex && sceneData.shadowAtlas)
		{
			const auto& shadowSampler = Graphics::Instance()->GetSamplerCache().Get({ .depthCompare = true });

			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::SampledTextureBinding{
				.texture = sceneData.shadowAtlas.get(),
				.sampler = shadowSampler.get()
			};
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotLights); bindingIndex != Material::InvalidBindingIndex && sceneData.spotLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.spotLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && sceneData.spotLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::WholeBuffer(*sceneData.spotLightAtlasMapping);
		}
	}

	ElementRendererData::~ElementRendererData() = default;
}
