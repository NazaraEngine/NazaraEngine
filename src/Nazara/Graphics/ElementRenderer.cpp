// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	ElementRenderer::~ElementRenderer() = default;

	void ElementRenderer::Prepare(const RenderData& /*renderData*/, const AbstractViewer& /*viewer*/, ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, std::size_t /*elementCount*/, const Pointer<const RenderElement>* /*elements*/)
	{
	}

	void ElementRenderer::PrepareEnd(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/, CommandBufferBuilder& /*commandBuffer*/)
	{
	}

	void ElementRenderer::Reset(ElementRendererData& /*rendererData*/, RenderResources& /*renderResources*/)
	{
	}

	void ElementRenderer::FillSceneBindings(const RenderData& renderData, const Material& material, std::vector<ShaderBinding::Binding>& bindings)
	{
		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalLights); bindingIndex != Material::InvalidBindingIndex && renderData.directionalLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.directionalLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.directionalLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.directionalLightAtlasMapping);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointLights); bindingIndex != Material::InvalidBindingIndex && renderData.pointLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.pointLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.pointLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.pointLightAtlasMapping);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowAtlas); bindingIndex != Material::InvalidBindingIndex && renderData.shadowAtlas)
		{
			const auto& shadowSampler = Graphics::Instance()->GetSamplerCache().Get({ .depthCompare = true });

			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::SampledTextureBinding{
				.texture = renderData.shadowAtlas,
				.sampler = shadowSampler.get()
			};
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotLights); bindingIndex != Material::InvalidBindingIndex && renderData.spotLights)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.spotLights);
		}

		if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.spotLightAtlasMapping)
		{
			auto& bindingEntry = bindings.emplace_back();
			bindingEntry.bindingIndex = bindingIndex;
			bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.spotLightAtlasMapping);
		}
	}

	ElementRendererData::~ElementRendererData() = default;
}
