// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/BlitPipelinePass.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/FramePipeline.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>

namespace Nz
{
	void BlitPipelinePass::Prepare(FrameData& /*frameData*/)
	{
		// nothing to do
	}

	FramePass& BlitPipelinePass::RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs)
	{
		if (inputOuputs.inputAttachments.size() != 1)
			throw std::runtime_error("one input expected");

		if (inputOuputs.outputAttachments.size() != 1)
			throw std::runtime_error("one output expected");

		std::size_t inputAttachment = inputOuputs.inputAttachments[0].attachmentIndex;
		std::size_t outputAttachment = inputOuputs.outputAttachments[0].attachmentIndex;

		FramePass& blitPass = frameGraph.AddPass("Blit pass");
		blitPass.AddInput(inputAttachment);
		blitPass.SetInputAccess(0, TextureLayout::TransferSource, PipelineStage::Transfer, MemoryAccess::MemoryRead);
		blitPass.SetInputUsage(0, TextureUsage::TransferSource);
		blitPass.SetReadInput(0, false);

		blitPass.AddOutput(outputAttachment);
		blitPass.SetOutputAccess(0, TextureLayout::TransferDestination, PipelineStage::Transfer, MemoryAccess::MemoryWrite);
		blitPass.SetOutputUsage(0, TextureUsage::TransferDestination);

		blitPass.SetCommandCallback([this, inputAttachment, outputAttachment](CommandBufferBuilder& builder, const FramePassEnvironment& env)
		{
			const std::shared_ptr<Texture>& sourceTexture = env.frameGraph.GetAttachmentTexture(inputAttachment);
			const std::shared_ptr<Texture>& destinationTexture = env.frameGraph.GetAttachmentTexture(outputAttachment);

			Vector2ui sourceTextureSize = Vector2ui(sourceTexture->GetSize());
			Vector2ui targetTextureSize = Vector2ui(destinationTexture->GetSize());

			Boxui fromBox(0, 0, 0, sourceTextureSize.x, sourceTextureSize.y, 1);
			Boxui toBox(0, 0, 0, targetTextureSize.x, targetTextureSize.y, 1);

			builder.TextureBarrier(PipelineStage::TopOfPipe, PipelineStage::Transfer, {}, MemoryAccess::TransferWrite, TextureLayout::Undefined, TextureLayout::TransferDestination, *destinationTexture);
			if (sourceTextureSize == targetTextureSize)
				builder.CopyTexture(*sourceTexture, fromBox, TextureLayout::TransferSource, *destinationTexture, toBox.GetPosition(), TextureLayout::TransferDestination);
			else
				builder.BlitTexture(*sourceTexture, fromBox, TextureLayout::TransferSource, *destinationTexture, toBox, TextureLayout::TransferDestination, SamplerFilter::Linear);
		});

		return blitPass;
	}
}
