// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderWindow.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderWindow::RenderWindow(WindowSwapchain& swapchain) :
	m_swapchain(nullptr),
	m_windowSwapchain(&swapchain)
	{
		m_onSwapchainCreated.Connect(swapchain.OnSwapchainCreated, [this](WindowSwapchain* /*windowSwapchain*/, Swapchain& swapchain)
		{
			SetSwapchain(&swapchain);
		});

		m_onSwapchainDestroy.Connect(swapchain.OnSwapchainDestroy, [this](WindowSwapchain* /*windowSwapchain*/)
		{
			SetSwapchain(nullptr);
		});

		SetSwapchain(m_windowSwapchain->GetSwapchain());
	}

	void RenderWindow::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		graph.AddOutput(attachmentIndex);
	}

	void RenderWindow::OnRenderEnd(RenderFrame& renderFrame, const BakedFrameGraph& frameGraph, std::size_t finalAttachment) const
	{
		const std::shared_ptr<Texture>& texture = frameGraph.GetAttachmentTexture(finalAttachment);

		Vector2ui textureSize = Vector2ui(texture->GetSize());
		Boxui blitRegion(0, 0, 0, textureSize.x, textureSize.y, 1);

		renderFrame.Execute([&](CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Blit to swapchain", Color::Blue());
			{
				builder.TextureBarrier(PipelineStage::ColorOutput, PipelineStage::Transfer, MemoryAccess::ColorWrite, MemoryAccess::TransferRead, TextureLayout::ColorOutput, TextureLayout::TransferSource, *texture);
				builder.BlitTextureToSwapchain(*texture, blitRegion, TextureLayout::TransferSource, *m_swapchain, renderFrame.GetFramebufferIndex());
			}
			builder.EndDebugRegion();
		}, QueueType::Graphics);
	}

	const Vector2ui& RenderWindow::GetSize() const
	{
		if (m_swapchain)
			return m_swapchain->GetSize();
		else if (m_windowSwapchain)
			return m_windowSwapchain->GetSize();
		else
		{
			static Vector2ui dummySize(1, 1);
			return dummySize;
		}
	}

	void RenderWindow::SetSwapchain(Swapchain* swapchain)
	{
		m_swapchain = swapchain;
		if (m_swapchain)
		{
			OnRenderTargetSizeChange(this, m_swapchain->GetSize());

			m_onSwapchainResize.Connect(m_swapchain->OnSwapchainResize, [this]([[maybe_unused]] Swapchain* swapchain, const Vector2ui& newSize)
			{
				assert(m_swapchain == swapchain);
				OnRenderTargetSizeChange(this, newSize);
			});
		}
	}
}
