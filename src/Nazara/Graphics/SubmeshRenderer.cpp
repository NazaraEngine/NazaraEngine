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

		const RenderBuffer* currentIndexBuffer = nullptr;
		const RenderBuffer* currentVertexBuffer = nullptr;
		const MaterialInstance* currentMaterialInstance = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentSceneShaderBinding = nullptr;
		const ShaderBinding* currentViewerShaderBinding = nullptr;
		const ShaderBinding* currentMaterialShaderBinding = nullptr;
		const ShaderBinding* currentInstanceShaderBinding = nullptr;
		const SkeletonInstance* currentSkeletonInstance = nullptr;
		const WorldInstance* currentWorldInstance = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		std::size_t sceneBindingHash = 0;
		std::size_t viewerBindingHash = 0;

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
				currentMaterialInstance = materialInstance;
				currentMaterialShaderBinding = nullptr;
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
				currentInstanceShaderBinding = nullptr;
				currentSkeletonInstance = skeletonInstance;
			}

			if (const WorldInstance* worldInstance = &submesh.GetWorldInstance(); currentWorldInstance != worldInstance)
			{
				currentInstanceShaderBinding = nullptr;
				currentWorldInstance = worldInstance;
			}

			const Recti& scissorBox = submesh.GetScissorBox();
			const Recti& targetScissorBox = (scissorBox.width >= 0) ? scissorBox : renderData.renderRegion;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			const Material& material = *currentMaterialInstance->GetParentMaterial();

			if (!currentSceneShaderBinding || sceneBindingHash != material.GetBindingSetHash(Material::SceneBindingSet))
			{
				ShaderBindingPtr sceneBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::SceneBindingSet);

				m_bindingCache.clear();
				FillSceneBindings(renderData, material, m_bindingCache);
				sceneBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(Material::SceneBindingSet, *sceneBinding);

				currentSceneShaderBinding = sceneBinding.get();

				data.shaderBindings.emplace_back(std::move(sceneBinding));
				sceneBindingHash = material.GetBindingSetHash(Material::SceneBindingSet);

				currentViewerShaderBinding = nullptr;
			}

			if (!currentViewerShaderBinding || viewerBindingHash != material.GetBindingSetHash(Material::ViewerBindingSet))
			{
				ShaderBindingPtr viewerBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::ViewerBindingSet);

				m_bindingCache.clear();
				viewer.GetViewerInstance().FillShaderBinding(material, *data.references, m_bindingCache);
				viewerBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(Material::ViewerBindingSet, *viewerBinding);

				currentViewerShaderBinding = viewerBinding.get();

				data.shaderBindings.emplace_back(std::move(viewerBinding));
				viewerBindingHash = material.GetBindingSetHash(Material::ViewerBindingSet);

				currentMaterialShaderBinding = nullptr;
			}

			if (!currentMaterialShaderBinding)
			{
				ShaderBindingPtr materialBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::MaterialBindingSet);

				m_bindingCache.clear();
				currentMaterialInstance->FillShaderBinding(*data.references, m_bindingCache);
				materialBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(Material::MaterialBindingSet, *materialBinding);

				currentMaterialShaderBinding = materialBinding.get();

				data.shaderBindings.emplace_back(std::move(materialBinding));

				currentInstanceShaderBinding = nullptr;
			}

			if (!currentInstanceShaderBinding)
			{
				ShaderBindingPtr instanceBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(Material::InstanceBindingSet);

				m_bindingCache.clear();
				currentWorldInstance->FillShaderBinding(material, *data.references, m_bindingCache);

				if (currentSkeletonInstance)
					currentSkeletonInstance->FillShaderBinding(material, *data.references, m_bindingCache);

				instanceBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				commandBuffer.BindRenderShaderBinding(Material::InstanceBindingSet, *instanceBinding);

				currentInstanceShaderBinding = instanceBinding.get();

				data.shaderBindings.emplace_back(std::move(instanceBinding));
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
