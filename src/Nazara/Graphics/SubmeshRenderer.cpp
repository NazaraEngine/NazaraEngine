// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>

namespace Nz
{
	SubmeshRenderer::SubmeshRenderer()
	{
		m_pool = std::make_shared<PoolData>();
	}

	RenderElementPool<RenderSubmesh>& SubmeshRenderer::GetPool()
	{
		return m_submeshPool;
	}

	std::unique_ptr<ElementRendererData> SubmeshRenderer::InstanciateData()
	{
		return std::make_unique<SubmeshRendererData>();
	}

	void SubmeshRenderer::Render(const RenderData& renderData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& /*renderResources*/, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements)
	{
		Graphics* graphics = Graphics::Instance();
		auto& renderDevice = *graphics->GetRenderDevice();

		auto& data = static_cast<SubmeshRendererData&>(rendererData);
		if (!data.references)
		{
			if (!m_pool->references.empty())
			{
				data.references = std::move(m_pool->references.back());
				m_pool->references.pop_back();
			}
			else
				data.references.emplace();
		}

		Vector2f targetSize = viewer.GetViewerInstance().GetTargetSize();
		Recti fullscreenScissorBox(0, 0, SafeCast<int>(std::floor(targetSize.x)), SafeCast<int>(std::floor(targetSize.y)));

		const RenderBuffer* currentIndexBuffer = nullptr;
		const RenderBuffer* currentVertexBuffer = nullptr;
		const MaterialInstance* currentMaterialInstance = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;
		const SkeletonInstance* currentSkeletonInstance = nullptr;
		const WorldInstance* currentWorldInstance = nullptr;
		Recti currentScissorBox = fullscreenScissorBox;

		auto FlushDrawData = [&]()
		{
			currentShaderBinding = nullptr;
		};

		const auto& depthTexture2D = Graphics::Instance()->GetDefaultTextures().depthTextures[ImageType::E2D];
		const auto& depthTexture2DArray = Graphics::Instance()->GetDefaultTextures().depthTextures[ImageType::E2D_Array];
		const auto& depthTextureCube = Graphics::Instance()->GetDefaultTextures().depthTextures[ImageType::Cubemap];
		const auto& whiteTexture2D = Graphics::Instance()->GetDefaultTextures().whiteTextures[ImageType::E2D];
		const auto& defaultSampler = graphics->GetSamplerCache().Get({});
		const auto& shadowSampler = graphics->GetSamplerCache().Get({ .depthCompare = true });

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			NazaraAssert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::Submesh));
			const RenderSubmesh& submesh = static_cast<const RenderSubmesh&>(*elements[i]);

			if (const RenderPipeline* pipeline = submesh.GetRenderPipeline(); currentPipeline != pipeline)
			{
				commandBuffer.BindRenderPipeline(*pipeline);
				currentPipeline = pipeline;
			}

			if (const MaterialInstance* materialInstance = &submesh.GetMaterialInstance(); currentMaterialInstance != materialInstance)
			{
				FlushDrawData();
				currentMaterialInstance = materialInstance;
			}

			if (const RenderBuffer* indexBuffer = submesh.GetIndexBuffer(); currentIndexBuffer != indexBuffer)
			{
				if (indexBuffer)
					commandBuffer.BindIndexBuffer(*indexBuffer, submesh.GetIndexType());

				currentIndexBuffer = indexBuffer;
			}

			if (const RenderBuffer* vertexBuffer = submesh.GetVertexBuffer(); currentVertexBuffer != vertexBuffer)
			{
				if (vertexBuffer)
					commandBuffer.BindVertexBuffer(0, *vertexBuffer);

				currentVertexBuffer = vertexBuffer;
			}

			if (const SkeletonInstance* skeletonInstance = submesh.GetSkeletonInstance(); currentSkeletonInstance != skeletonInstance)
			{
				FlushDrawData();
				currentSkeletonInstance = skeletonInstance;
			}

			if (const WorldInstance* worldInstance = &submesh.GetWorldInstance(); currentWorldInstance != worldInstance)
			{
				FlushDrawData();
				currentWorldInstance = worldInstance;
			}

			const Recti& scissorBox = submesh.GetScissorBox();
			const Recti& targetScissorBox = (scissorBox.width >= 0) ? scissorBox : fullscreenScissorBox;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			if (!currentShaderBinding)
			{
				NazaraAssert(currentMaterialInstance);

				m_bindingCache.clear();
				m_textureBindingCache.clear();
				m_textureBindingCache.reserve(9);
				//m_textureBindingCache.reserve(renderState.shadowMapsSpot.size() + renderState.shadowMapsDirectional.size() + renderState.shadowMapsPoint.size());

				NazaraAssert(data.references);
				currentMaterialInstance->FillShaderBinding(*data.references, m_bindingCache);

				const Material& material = *currentMaterialInstance->GetParentMaterial();

				// Predefined shader bindings
				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalLights); bindingIndex != Material::InvalidBindingIndex && renderData.directionalLights)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.directionalLights);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::DirectionalShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.directionalLightAtlasMapping)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.directionalLightAtlasMapping);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointLights); bindingIndex != Material::InvalidBindingIndex && renderData.pointLights)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.pointLights);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::PointShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.pointLightAtlasMapping)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.pointLightAtlasMapping);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowAtlas); bindingIndex != Material::InvalidBindingIndex && renderData.shadowAtlas)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::SampledTextureBinding{
						.texture = renderData.shadowAtlas,
						.sampler = shadowSampler.get()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotLights); bindingIndex != Material::InvalidBindingIndex && renderData.spotLights)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.spotLights);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SpotShadowAtlasMapping); bindingIndex != Material::InvalidBindingIndex && renderData.spotLightAtlasMapping)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::StorageBufferBinding::FromView(renderData.spotLightAtlasMapping);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ViewerDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					const auto& viewerBuffer = viewer.GetViewerInstance().GetViewerBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding::WholeBuffer(*viewerBuffer);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::InstanceDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					NazaraAssert(currentWorldInstance);
					const auto& instanceBuffer = currentWorldInstance->GetInstanceBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding::WholeBuffer(*instanceBuffer);
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SkeletalDataUbo); bindingIndex != Material::InvalidBindingIndex && currentSkeletonInstance)
				{
					const auto& skeletalBuffer = currentSkeletonInstance->GetSkeletalBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding::WholeBuffer(*skeletalBuffer);
				}

				assert(currentPipeline);
				ShaderBindingPtr drawDataBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
				drawDataBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(0, *drawDataBinding);

				currentShaderBinding = drawDataBinding.get();

				data.shaderBindings.emplace_back(std::move(drawDataBinding));
			}

			if (currentIndexBuffer)
				commandBuffer.DrawIndexed(SafeCaster(submesh.GetIndexCount()), 1U, 0);
			else
				commandBuffer.Draw(SafeCaster(submesh.GetIndexCount()), 1U, 0);
		}
	}

	void SubmeshRenderer::Reset(ElementRendererData& rendererData, RenderResources& renderResources)
	{
		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		if (data.references)
		{
			renderResources.PushReleaseCallback([pool = m_pool, references = std::move(*data.references)]() mutable
			{
				references.Clear();
				pool->references.push_back(std::move(references));
			});
			data.references.reset();
		}

		for (auto& shaderBinding : data.shaderBindings)
			renderResources.PushForRelease(std::move(shaderBinding));
		data.shaderBindings.clear();
	}
}
