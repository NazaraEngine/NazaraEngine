// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderWindow.hpp>
#include <Nazara/Graphics/BakedFrameGraph.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderWindow::RenderWindow(WindowSwapchain& swapchain) :
	RenderTarget(DefaultRenderOrder),
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

		SetFrameGraphOutput(true);
		SetSwapchain(m_windowSwapchain->GetSwapchain());
	}

	std::size_t RenderWindow::OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const
	{
		// TODO: Replace the blit to swapchain by a graph.BindExternalSwapchain?
		std::size_t linkAttachment = graph.AddDummyAttachment();
		
		FramePass& blitPass = graph.AddPass("Blit to swapchain");
		blitPass.AddInput(attachmentIndex);
		blitPass.SetInputAccess(0, TextureLayout::TransferSource, PipelineStage::Transfer, MemoryAccess::MemoryRead);
		blitPass.SetInputUsage(0, TextureUsage::TransferSource);

		blitPass.AddOutput(linkAttachment);

		// Force regeneration of RenderWindow execution callback (since image index changes every frame)
		// TODO: Maybe handle this in a better way (temporary command buffer? multiple commands buffers selected by frame index?)
		blitPass.SetExecutionCallback([]
		{
			return FramePassExecution::UpdateAndExecute;
		});

		blitPass.SetCommandCallback([this, attachmentIndex](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			const std::shared_ptr<Texture>& sourceTexture = env.frameGraph.GetAttachmentTexture(attachmentIndex);

			Vector2ui textureSize = Vector2ui(sourceTexture->GetSize());
			Boxui blitRegion(0, 0, 0, textureSize.x, textureSize.y, 1);

			builder.BlitTextureToSwapchain(*sourceTexture, blitRegion, TextureLayout::TransferSource, *m_swapchain, env.renderResources.GetImageIndex());
		});

		return linkAttachment;
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
