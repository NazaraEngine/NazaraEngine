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

	void SubmeshRenderer::Render(const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& /*renderResources*/, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements, SparsePtr<const RenderStates> renderStates)
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
		RenderBufferView currentLightData;

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
			const RenderStates& renderState = renderStates[i];

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

			if (currentLightData != renderState.lightData)
			{
				FlushDrawData();
				currentLightData = renderState.lightData;
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
				m_textureBindingCache.reserve(renderState.shadowMapsSpot.size() + renderState.shadowMapsDirectional.size() + renderState.shadowMapsPoint.size());

				NazaraAssert(data.references);
				currentMaterialInstance->FillShaderBinding(*data.references, m_bindingCache);

				const Material& material = *currentMaterialInstance->GetParentMaterial();

				// Predefined shader bindings
				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::InstanceDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					NazaraAssert(currentWorldInstance);
					const auto& instanceBuffer = currentWorldInstance->GetInstanceBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						instanceBuffer.get(),
						0, instanceBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::LightDataUbo); bindingIndex != Material::InvalidBindingIndex && currentLightData)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						currentLightData.GetBuffer(),
						currentLightData.GetOffset(), currentLightData.GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowmapDirectional); bindingIndex != Material::InvalidBindingIndex)
				{
					std::size_t textureBindingBaseIndex = m_textureBindingCache.size();

					for (std::size_t j = 0; j < renderState.shadowMapsDirectional.size(); ++j)
					{
						const Texture* texture = renderState.shadowMapsDirectional[j];
						if (!texture)
							texture = depthTexture2DArray->GetOrCreateTexture(renderDevice).get();

						auto& textureEntry = m_textureBindingCache.emplace_back();
						textureEntry.texture = texture;
						textureEntry.sampler = shadowSampler.get();
					}

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::SampledTextureBindings {
						SafeCast<UInt32>(renderState.shadowMapsDirectional.size()), &m_textureBindingCache[textureBindingBaseIndex]
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowmapPoint); bindingIndex != Material::InvalidBindingIndex)
				{
					std::size_t textureBindingBaseIndex = m_textureBindingCache.size();

					for (std::size_t j = 0; j < renderState.shadowMapsPoint.size(); ++j)
					{
						const Texture* texture = renderState.shadowMapsPoint[j];
						if (!texture)
							texture = depthTextureCube->GetOrCreateTexture(renderDevice).get();

						auto& textureEntry = m_textureBindingCache.emplace_back();
						textureEntry.texture = texture;
						textureEntry.sampler = defaultSampler.get(); //< cube shadowmap don't use depth compare
					}

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::SampledTextureBindings {
						SafeCast<UInt32>(renderState.shadowMapsPoint.size()), &m_textureBindingCache[textureBindingBaseIndex]
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ShadowmapSpot); bindingIndex != Material::InvalidBindingIndex)
				{
					std::size_t textureBindingBaseIndex = m_textureBindingCache.size();

					for (std::size_t j = 0; j < renderState.shadowMapsSpot.size(); ++j)
					{
						const Texture* texture = renderState.shadowMapsSpot[j];
						if (!texture)
							texture = depthTexture2D->GetOrCreateTexture(renderDevice).get();

						auto& textureEntry = m_textureBindingCache.emplace_back();
						textureEntry.texture = texture;
						textureEntry.sampler = shadowSampler.get();
					}

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::SampledTextureBindings {
						SafeCast<UInt32>(renderState.shadowMapsSpot.size()), &m_textureBindingCache[textureBindingBaseIndex]
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SkeletalDataUbo); bindingIndex != Material::InvalidBindingIndex && currentSkeletonInstance)
				{
					const auto& skeletalBuffer = currentSkeletonInstance->GetSkeletalBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						skeletalBuffer.get(),
						0, skeletalBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ViewerDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					const auto& viewerBuffer = viewer.GetViewerInstance().GetViewerBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						viewerBuffer.get(),
						0, viewerBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::OverlayTexture); bindingIndex != Material::InvalidBindingIndex)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::SampledTextureBinding{
						whiteTexture2D->GetOrCreateTexture(renderDevice).get(), defaultSampler.get()
					};
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
