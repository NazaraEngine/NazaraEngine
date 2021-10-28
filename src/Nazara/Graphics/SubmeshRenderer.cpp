// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	SubmeshRenderer::SubmeshRenderer()
	{
	}

	std::unique_ptr<ElementRendererData> SubmeshRenderer::InstanciateData()
	{
		return std::make_unique<SubmeshRendererData>();
	}

	void SubmeshRenderer::Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& /*currentFrame*/, const Pointer<const RenderElement>* elements, std::size_t elementCount)
	{
		Graphics* graphics = Graphics::Instance();

		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		const AbstractBuffer* currentIndexBuffer = nullptr;
		const AbstractBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;
		const WorldInstance* currentWorldInstance = nullptr;

		auto FlushDrawCall = [&]()
		{
			// Does nothing for now (but will serve once instancing is implemented)
		};

		auto FlushDrawData = [&]()
		{
			FlushDrawCall();

			currentShaderBinding = nullptr;
		};

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTextures[UnderlyingCast(ImageType::E2D)];
		const auto& defaultSampler = graphics->GetSamplerCache().Get({});

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			assert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::Submesh));
			const RenderSubmesh& submesh = static_cast<const RenderSubmesh&>(*elements[i]);

			if (const RenderPipeline* pipeline = submesh.GetRenderPipeline(); currentPipeline != pipeline)
			{
				FlushDrawCall();
				currentPipeline = pipeline;
			}

			if (const AbstractBuffer* indexBuffer = submesh.GetIndexBuffer(); currentIndexBuffer != indexBuffer)
			{
				FlushDrawCall();
				currentIndexBuffer = indexBuffer;
			}

			if (const AbstractBuffer* vertexBuffer = submesh.GetVertexBuffer(); currentVertexBuffer != vertexBuffer)
			{
				FlushDrawCall();
				currentVertexBuffer = vertexBuffer;
			}

			if (currentWorldInstance != &submesh.GetWorldInstance())
			{
				// TODO: Flushing draw calls on instance binding means we can have e.g. 1000 sprites rendered using a draw call for each one
				// which is far from being efficient, using some bindless could help (or at least instancing?)
				FlushDrawData();
				currentWorldInstance = &submesh.GetWorldInstance();
			}

			if (!currentShaderBinding)
			{
				m_bindingCache.clear();

				const MaterialPass& materialPass = submesh.GetMaterialPass();
				materialPass.FillShaderBinding(m_bindingCache);

				// Predefined shader bindings
				const auto& matSettings = materialPass.GetSettings();
				if (std::size_t bindingIndex = matSettings->GetPredefinedBinding(PredefinedShaderBinding::InstanceDataUbo); bindingIndex != MaterialSettings::InvalidIndex)
				{
					const auto& instanceBuffer = currentWorldInstance->GetInstanceBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						instanceBuffer.get(),
						0, instanceBuffer->GetSize()
					};
				}

				if (std::size_t bindingIndex = matSettings->GetPredefinedBinding(PredefinedShaderBinding::ViewerDataUbo); bindingIndex != MaterialSettings::InvalidIndex)
				{
					const auto& viewerBuffer = viewerInstance.GetViewerBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						viewerBuffer.get(),
						0, viewerBuffer->GetSize()
					};
				}

				if (std::size_t bindingIndex = matSettings->GetPredefinedBinding(PredefinedShaderBinding::OverlayTexture); bindingIndex != MaterialSettings::InvalidIndex)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::TextureBinding{
						whiteTexture.get(), defaultSampler.get()
					};
				}

				ShaderBindingPtr drawDataBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
				drawDataBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				currentShaderBinding = drawDataBinding.get();

				data.shaderBindings.emplace_back(std::move(drawDataBinding));
			}

			auto& drawCall = data.drawCalls.emplace_back();
			drawCall.indexBuffer = currentIndexBuffer;
			drawCall.indexCount = submesh.GetIndexCount();
			drawCall.renderPipeline = currentPipeline;
			drawCall.shaderBinding = currentShaderBinding;
			drawCall.vertexBuffer = currentVertexBuffer;
		}
	}

	void SubmeshRenderer::Render(const ViewerInstance& /*viewerInstance*/, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* /*elements*/, std::size_t /*elementCount*/)
	{
		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		const AbstractBuffer* currentIndexBuffer = nullptr;
		const AbstractBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;

		for (const auto& drawData : data.drawCalls)
		{
			if (currentPipeline != drawData.renderPipeline)
			{
				commandBuffer.BindPipeline(*drawData.renderPipeline);
				currentPipeline = drawData.renderPipeline;
			}

			if (currentShaderBinding != drawData.shaderBinding)
			{
				commandBuffer.BindShaderBinding(0, *drawData.shaderBinding);
				currentShaderBinding = drawData.shaderBinding;
			}

			if (currentIndexBuffer != drawData.indexBuffer)
			{
				commandBuffer.BindIndexBuffer(*drawData.indexBuffer);
				currentIndexBuffer = drawData.indexBuffer;
			}

			if (currentVertexBuffer != drawData.vertexBuffer)
			{
				commandBuffer.BindVertexBuffer(0, *drawData.vertexBuffer);
				currentVertexBuffer = drawData.vertexBuffer;
			}

			if (currentIndexBuffer)
				commandBuffer.DrawIndexed(drawData.indexCount);
			else
				commandBuffer.Draw(drawData.indexCount);
		}
	}

	void SubmeshRenderer::Reset(ElementRendererData& rendererData, RenderFrame& currentFrame)
	{
		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		for (auto& shaderBinding : data.shaderBindings)
			currentFrame.PushForRelease(std::move(shaderBinding));
		data.shaderBindings.clear();

		data.drawCalls.clear();
	}
}
