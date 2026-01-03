// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// This class was written with a lot of help from themaister articles and Granite source code, check them out!
// https://themaister.net/blog/2017/08/15/render-graphs-and-vulkan-a-deep-dive/

#include <Nazara/Graphics/FrameGraph.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <stdexcept>

namespace Nz
{
	namespace
	{
		template<typename T> void UniquePushBack(std::vector<T>& vec, const T& value)
		{
			auto it = std::find(vec.begin(), vec.end(), value);
			if (it == vec.end())
				vec.push_back(value);
		}
	}

	BakedFrameGraph FrameGraph::Bake()
	{
		if (m_graphOutputs.empty())
			throw std::runtime_error("no graph output has been set");

		m_pending.attachmentReadList.clear();
		m_pending.attachmentToTextures.clear();
		m_pending.attachmentWriteList.clear();
		m_pending.barrierList.clear();
		m_pending.passIdToPhysicalPassIndex.clear();
		m_pending.passList.clear();
		m_pending.physicalPasses.clear();
		m_pending.renderPasses.clear();
		m_pending.textures.clear();
		m_pending.texture2DPool.clear();
		m_pending.textureCubePool.clear();

		BuildReadWriteList();

		for (std::size_t output : m_graphOutputs)
		{
			auto it = m_pending.attachmentWriteList.find(output);
			if (it == m_pending.attachmentWriteList.end())
				throw std::runtime_error("no pass writes to backbuffer");

			const std::vector<std::size_t>& backbufferPasses = it->second;
			for (std::size_t passIndex : backbufferPasses)
				TraverseGraph(passIndex);
		}

		std::reverse(m_pending.passList.begin(), m_pending.passList.end());

		RemoveDuplicatePasses();
		ReorderPasses();
		AssignPhysicalTextures();
		AssignPhysicalPasses();
		BuildPhysicalPasses();
		BuildBarriers();
		BuildPhysicalBarriers();

		std::vector<BakedFrameGraph::PassData> bakedPasses;
		bakedPasses.reserve(m_pending.physicalPasses.size());

		std::size_t renderPassIndex = 0;
		for (auto& physicalPass : m_pending.physicalPasses)
		{
			auto& bakedPass = bakedPasses.emplace_back();
			bakedPass.name = std::move(physicalPass.name);
			bakedPass.renderPass = std::move(m_pending.renderPasses[renderPassIndex++]);
			bakedPass.invalidationBarriers = std::move(physicalPass.textureBarrier);

			for (auto& subpass : physicalPass.passes)
			{
				const FramePass& framePass = m_framePasses[subpass.passIndex];
				bakedPass.executionCallback = framePass.GetExecutionCallback(); //< FIXME

				auto& bakedSubpass = bakedPass.subpasses.emplace_back();
				bakedSubpass.commandCallback = framePass.GetCommandCallback();

				std::size_t colorAttachmentIndex = 0;
				for (const auto& output : framePass.GetOutputs())
				{
					if (!output.textureUsageFlags.Test(TextureUsage::ColorAttachment))
						continue;

					if (std::size_t textureIndex = Retrieve(m_pending.attachmentToTextures, output.attachmentId); textureIndex != InvalidTextureIndex)
						bakedPass.outputTextureIndices.push_back(textureIndex);

					if (output.clearColor)
					{
						bakedPass.outputClearValues.resize(colorAttachmentIndex + 1);
						bakedPass.outputClearValues[colorAttachmentIndex].color = *output.clearColor;
					}

					colorAttachmentIndex++;
				}

				// Add depth-stencil clear values
				if (const auto& depthStencilClear = framePass.GetDepthStencilClear())
				{
					bakedPass.outputClearValues.resize(colorAttachmentIndex + 1);

					auto& dsClearValues = bakedPass.outputClearValues[colorAttachmentIndex];
					dsClearValues.depth = depthStencilClear->depth;
					dsClearValues.stencil = depthStencilClear->stencil;
				}

				if (std::size_t attachmentId = framePass.GetDepthStencilOutput(); attachmentId != FramePass::InvalidAttachmentId)
				{
					if (std::size_t textureIndex = Retrieve(m_pending.attachmentToTextures, attachmentId); textureIndex != InvalidTextureIndex)
						bakedPass.outputTextureIndices.push_back(textureIndex);
				}
				else if (const auto& depthStencilInput = framePass.GetDepthStencilInput())
				{
					if (std::size_t textureIndex = Retrieve(m_pending.attachmentToTextures, depthStencilInput->attachmentId); textureIndex != InvalidTextureIndex)
						bakedPass.outputTextureIndices.push_back(textureIndex);
				}
			}
		}

		std::vector<BakedFrameGraph::TextureData> bakedTextures;
		bakedTextures.reserve(m_pending.textures.size());
		for (auto& texture : m_pending.textures)
		{
			auto& bakedTexture = bakedTextures.emplace_back();
			static_cast<FrameGraphTextureData&>(bakedTexture) = std::move(texture);
			bakedTexture.texture = bakedTexture.externalTexture;
		}

		return BakedFrameGraph(std::move(bakedPasses), std::move(bakedTextures), std::move(m_pending.attachmentToTextures), std::move(m_pending.passIdToPhysicalPassIndex));
	}

	void FrameGraph::AssignPhysicalPasses()
	{
		auto ShouldMerge = [&](const FramePass& prevPass, const FramePass& nextPass)
		{
			//TODO
			return false;
		};

		for (std::size_t passIndex = 0; passIndex < m_pending.passList.size();)
		{
			std::size_t mergeEnd = passIndex + 1;
			for (; mergeEnd < m_pending.passList.size(); ++mergeEnd)
			{
				bool merge = true;
				for (std::size_t mergeStart = passIndex; mergeStart < mergeEnd; ++mergeStart)
				{
					if (!ShouldMerge(m_framePasses[m_pending.passList[mergeStart]], m_framePasses[m_pending.passList[mergeEnd]]))
					{
						merge = false;
						break;
					}
				}

				if (!merge)
					break;
			}

			std::size_t physPassIndex = m_pending.physicalPasses.size();
			PhysicalPassData& currentPass = m_pending.physicalPasses.emplace_back();

			auto it = m_pending.passList.begin() + passIndex;
			auto end = m_pending.passList.begin() + mergeEnd;

			for (; it < end; ++it)
			{
				const FramePass& pass = m_framePasses[*it];
				if (currentPass.name.empty())
					currentPass.name = pass.GetName();
				else
					currentPass.name += " / " + pass.GetName();

				auto& subpass = currentPass.passes.emplace_back();
				subpass.passIndex = *it;
				m_pending.passIdToPhysicalPassIndex.emplace(subpass.passIndex, physPassIndex);
			}

			passIndex = mergeEnd;
		}
	}

	void FrameGraph::AssignPhysicalTextures()
	{
		// Assign last use pass index for every attachment
		for (std::size_t passIndex : m_pending.passList)
		{
			const FramePass& framePass = m_framePasses[passIndex];
			framePass.ForEachAttachment([&](std::size_t attachmentId)
			{
				attachmentId = ResolveAttachmentIndex(attachmentId);
				m_pending.attachmentLastUse[attachmentId] = passIndex;
			});
		}

		for (std::size_t passIndex : m_pending.passList)
		{
			const FramePass& framePass = m_framePasses[passIndex];

			for (const auto& input : framePass.GetInputs())
			{
				std::size_t textureId = RegisterTexture(input.attachmentId);
				if (textureId != InvalidTextureIndex)
				{
					FrameGraphTextureData& attachmentData = m_pending.textures[textureId];
					attachmentData.usage |= input.textureUsageFlags;
				}
			}

			for (const auto& output : framePass.GetOutputs())
			{
				std::size_t textureId = RegisterTexture(output.attachmentId);
				if (textureId != InvalidTextureIndex)
				{
					FrameGraphTextureData& attachmentData = m_pending.textures[textureId];
					attachmentData.usage |= output.textureUsageFlags;
				}
			}

			if (const auto& depthStencilInput = framePass.GetDepthStencilInput())
			{
				std::size_t textureId = RegisterTexture(depthStencilInput->attachmentId);
				if (textureId != InvalidTextureIndex)
				{
					FrameGraphTextureData& attachmentData = m_pending.textures[textureId];
					attachmentData.usage |= depthStencilInput->textureUsageFlags;
				}

				if (std::size_t depthStencilOutput = framePass.GetDepthStencilOutput(); depthStencilOutput != FramePass::InvalidAttachmentId)
				{
					if (auto it = m_pending.attachmentToTextures.find(depthStencilOutput); it == m_pending.attachmentToTextures.end())
					{
						// Special case where multiples attachments point simultaneously to the same texture
						m_pending.attachmentToTextures.emplace(depthStencilOutput, textureId);

						auto inputIt = m_pending.attachmentLastUse.find(depthStencilInput->attachmentId);
						auto outputIt = m_pending.attachmentLastUse.find(depthStencilInput->attachmentId);
						if (inputIt != m_pending.attachmentLastUse.end() && outputIt != m_pending.attachmentLastUse.end())
						{
							if (inputIt->second > outputIt->second)
								m_pending.attachmentLastUse.erase(outputIt);
							else
								m_pending.attachmentLastUse.erase(inputIt);
						}
					}
					else if (it->second != textureId)
						throw std::runtime_error("depth-stencil output already assigned");
				}
			}

			if (std::size_t depthStencilOutput = framePass.GetDepthStencilOutput(); depthStencilOutput != FramePass::InvalidAttachmentId)
			{
				std::size_t textureId = RegisterTexture(depthStencilOutput);

				FrameGraphTextureData& attachmentData = m_pending.textures[textureId];
				attachmentData.usage |= TextureUsage::DepthStencilAttachment;
			}

			framePass.ForEachAttachment([&](std::size_t attachmentId)
			{
				attachmentId = ResolveAttachmentIndex(attachmentId);

				auto it = m_pending.attachmentLastUse.find(attachmentId);

				// If this pass is the last one where this attachment is used, push the texture to the reuse pool
				if (it != m_pending.attachmentLastUse.end() && passIndex == it->second)
				{
					const auto& attachmentData = m_attachments[attachmentId];
					if (std::holds_alternative<FramePassAttachment>(attachmentData))
					{
						std::size_t textureId = Retrieve(m_pending.attachmentToTextures, attachmentId);
						if (m_pending.textures[textureId].canReuse)
						{
							assert(std::find(m_pending.texture2DPool.begin(), m_pending.texture2DPool.end(), textureId) == m_pending.texture2DPool.end());
							m_pending.texture2DPool.push_back(textureId);
						}
					}
					else if (std::holds_alternative<AttachmentArray>(attachmentData))
					{
						std::size_t textureId = Retrieve(m_pending.attachmentToTextures, attachmentId);
						if (m_pending.textures[textureId].canReuse)
						{
							assert(std::find(m_pending.textureCubePool.begin(), m_pending.textureCubePool.end(), textureId) == m_pending.textureCubePool.end());
							m_pending.texture2DArrayPool.push_back(textureId);
						}
					}
					else if (std::holds_alternative<AttachmentCube>(attachmentData))
					{
						std::size_t textureId = Retrieve(m_pending.attachmentToTextures, attachmentId);
						if (m_pending.textures[textureId].canReuse)
						{
							assert(std::find(m_pending.textureCubePool.begin(), m_pending.textureCubePool.end(), textureId) == m_pending.textureCubePool.end());
							m_pending.textureCubePool.push_back(textureId);
						}
					}
				}
			});
		}

		// Add TextureUsage::ShaderSampling and TextureUsage::TransferSource to final outputs
		for (std::size_t output : m_graphOutputs)
		{
			auto it = m_pending.attachmentToTextures.find(output);
			assert(it != m_pending.attachmentToTextures.end());

			if (std::size_t textureIndex = it->second; textureIndex != InvalidTextureIndex)
			{
				auto& finalTexture = m_pending.textures[textureIndex];
				finalTexture.usage |= TextureUsage::ShaderSampling | TextureUsage::TransferSource;
			}
		}

		// Apply texture view usage to their parents
		for (auto& textureData : m_pending.textures)
		{
			if (textureData.viewData)
			{
				auto& parentTextureData = m_pending.textures[textureData.viewData->parentTextureId];
				parentTextureData.usage |= textureData.usage;
			}
		}
	}

	void FrameGraph::BuildBarriers()
	{
		assert(m_pending.barrierList.empty());
		m_pending.barrierList.reserve(m_pending.passList.size());

		auto GetBarrier = [&](std::vector<Barrier>& barriers, std::size_t attachmentId) -> Barrier*
		{
			std::size_t textureId = Retrieve(m_pending.attachmentToTextures, ResolveAttachmentIndex(attachmentId));
			if (textureId == InvalidTextureIndex)
				return nullptr;

			// For texture view, use the parent texture layout (only if we're targeting all layers)
			if (m_pending.textures[textureId].viewData)
			{
				const auto& textureViewData = m_pending.textures[textureId].viewData;
				const auto& parentTexture = m_pending.textures[textureViewData->parentTextureId];
				UInt32 layerCount = (parentTexture.type == ImageType::Cubemap) ? 6 : parentTexture.layerCount;
				if (textureViewData->layerCount == layerCount)
					textureId = m_pending.textures[textureId].viewData->parentTextureId;
			}

			auto it = std::find_if(barriers.begin(), barriers.end(), [&](const Barrier& barrier) { return barrier.textureId == textureId; });
			if (it != barriers.end())
				return &*it;
			else
			{
				// Insert a new barrier
				auto& barrier = barriers.emplace_back();
				barrier.textureId = textureId;
				barrier.layout = TextureLayout::Undefined;

				return &barrier;
			}
		};

		for (std::size_t passId : m_pending.passList)
		{
			const FramePass& framePass = m_framePasses[passId];

			auto& barriers = m_pending.barrierList.emplace_back();

			auto GetInvalidationBarrier = [&](std::size_t attachmentId) -> Barrier* { return GetBarrier(barriers.invalidationBarriers, attachmentId); };
			auto GetFlushBarrier = [&](std::size_t attachmentId) -> Barrier* { return GetBarrier(barriers.flushBarriers, attachmentId); };

			for (const auto& input : framePass.GetInputs())
			{
				Barrier* barrier = GetInvalidationBarrier(input.attachmentId);
				if (!barrier)
					continue;

				if (barrier->layout != TextureLayout::Undefined && barrier->layout != input.layout)
					throw std::runtime_error("layout mismatch");

				barrier->access |= input.accessFlags;
				barrier->stages |= input.stageFlags;
				barrier->layout = input.layout;
			}

			for (const auto& output : framePass.GetOutputs())
			{
				Barrier* barrier = GetFlushBarrier(output.attachmentId);
				if (!barrier)
					continue;

				if (barrier->layout != TextureLayout::Undefined && barrier->layout != output.layout)
					throw std::runtime_error("layout mismatch");

				barrier->access |= output.accessFlags;
				barrier->stages |= output.stageFlags;
				barrier->layout = output.layout;
			}

			if (const auto& dsInput = framePass.GetDepthStencilInput())
			{
				// DS input(/output)
				if (Barrier* invalidationBarrier = GetInvalidationBarrier(dsInput->attachmentId))
				{
					TextureLayout depthStencilLayout;
					MemoryAccessFlags access = MemoryAccess::DepthStencilRead;
					PipelineStageFlags stages = PipelineStage::FragmentTestsEarly;

					if (std::size_t dsOutputAttachement = framePass.GetDepthStencilOutput())
					{
						NazaraAssertMsg(ResolveAttachmentIndex(dsOutputAttachement) == ResolveAttachmentIndex(dsInput->attachmentId), "it's not possible to use a different input and output depth buffer");
						depthStencilLayout = GetWriteDepthStencilLayout(dsOutputAttachement);
						access |= MemoryAccess::DepthStencilWrite;
						stages |= PipelineStage::FragmentTestsLate;
					}
					else
						depthStencilLayout = TextureLayout::DepthStencilReadOnly;

					if (invalidationBarrier->layout != TextureLayout::Undefined && invalidationBarrier->layout != depthStencilLayout)
						throw std::runtime_error("layout mismatch");

					invalidationBarrier->layout = depthStencilLayout;
					invalidationBarrier->access |= access;
					invalidationBarrier->stages |= stages;
				}
			}
			else if (std::size_t dsOutputAttachement = framePass.GetDepthStencilOutput(); dsOutputAttachement != FramePass::InvalidAttachmentId)
			{
				// DS output-only
				if (Barrier* flushBarrier = GetFlushBarrier(dsOutputAttachement))
				{
					if (flushBarrier->layout != TextureLayout::Undefined)
						throw std::runtime_error("layout mismatch");

					flushBarrier->layout = GetWriteDepthStencilLayout(dsOutputAttachement);
					flushBarrier->access = MemoryAccess::DepthStencilWrite;
					flushBarrier->stages = PipelineStage::FragmentTestsLate;
				}
			}
		}
	}

	void FrameGraph::BuildPhysicalBarriers()
	{
		struct PassTextureStates
		{
			MemoryAccessFlags invalidatedAccesses;
			MemoryAccessFlags flushedAccesses;
			PipelineStageFlags invalidatedStages;
			PipelineStageFlags flushedStages;
			TextureLayout initialLayout = TextureLayout::Undefined;
			TextureLayout finalLayout = TextureLayout::Undefined;
		};

		struct TextureStates
		{
			MemoryAccessFlags flushedAccesses;
			PipelineStageFlags flushedStages;
			TextureLayout currentLayout = TextureLayout::Undefined;
		};

		std::vector<TextureStates> textureStates(m_pending.textures.size());
		std::vector<PassTextureStates> passTextureStates;

		auto barriersIt = m_pending.barrierList.begin();
		for (auto& physicalPass : m_pending.physicalPasses)
		{
			passTextureStates.clear();
			passTextureStates.resize(m_pending.textures.size());

			for (auto& subpass : physicalPass.passes)
			{
				auto& barriers = *barriersIt++;

				for (auto& invalidation : barriers.invalidationBarriers)
				{
					auto& states = passTextureStates[invalidation.textureId];

					if (states.initialLayout == TextureLayout::Undefined)
					{
						// First use in this pass

						states.invalidatedAccesses |= invalidation.access;
						states.invalidatedStages |= invalidation.stages;
						states.initialLayout = invalidation.layout;
					}

					states.finalLayout = invalidation.layout;
					states.flushedAccesses = 0;
					states.flushedStages = 0;
				}

				for (auto& flush : barriers.flushBarriers)
				{
					auto& states = passTextureStates[flush.textureId];
					states.flushedAccesses |= flush.access;
					states.flushedStages |= flush.stages;
					states.finalLayout = flush.layout;

					if (states.initialLayout == TextureLayout::Undefined)
					{
						// First flush in a render pass needs a matching invalidation
						states.initialLayout = flush.layout;
						states.invalidatedAccesses = flush.access;
						states.invalidatedStages = flush.stages;

						textureStates[flush.textureId].currentLayout = flush.layout;

						if (states.invalidatedStages & PipelineStage::FragmentTestsLate)
							states.invalidatedStages |= PipelineStage::FragmentTestsEarly;

						if (states.invalidatedAccesses & MemoryAccess::ColorWrite)
							states.invalidatedAccesses |= MemoryAccess::ColorRead;

						if (states.invalidatedAccesses & MemoryAccess::DepthStencilWrite)
							states.invalidatedAccesses |= MemoryAccess::DepthStencilRead;

						if (states.invalidatedAccesses & MemoryAccess::ShaderWrite)
							states.invalidatedAccesses |= MemoryAccess::ShaderRead;

						// TODO: Discard resource
					}
				}
			}


			for (std::size_t textureId = 0; textureId < passTextureStates.size(); ++textureId)
			{
				const auto& state = passTextureStates[textureId];

				if (state.initialLayout == TextureLayout::Undefined && state.finalLayout == TextureLayout::Undefined)
					continue; //< Texture wasn't touched in this pass

				assert(state.finalLayout != TextureLayout::Undefined);

				if (textureStates[textureId].flushedAccesses != 0)
				{
					auto& invalidationBarrier = physicalPass.textureBarrier.emplace_back();
					invalidationBarrier.textureId = textureId;
					invalidationBarrier.srcAccessMask = textureStates[textureId].flushedAccesses;
					invalidationBarrier.srcStageMask = textureStates[textureId].flushedStages;
					invalidationBarrier.dstAccessMask = state.invalidatedAccesses;
					invalidationBarrier.dstStageMask = state.invalidatedStages;
					invalidationBarrier.oldLayout = textureStates[textureId].currentLayout;
					invalidationBarrier.newLayout = state.initialLayout;

					textureStates[textureId].flushedAccesses = 0;
					textureStates[textureId].flushedStages = 0;
				}

				textureStates[textureId].currentLayout = state.finalLayout;
				textureStates[textureId].flushedAccesses |= state.flushedAccesses;
				textureStates[textureId].flushedStages |= state.flushedStages;
			}
		}
	}

	void FrameGraph::BuildPhysicalPassDependencies(std::size_t colorAttachmentCount, bool hasDepthStencilAttachment, std::vector<RenderPass::Attachment>& renderPassAttachments, std::vector<RenderPass::SubpassDescription>& subpasses, std::vector<RenderPass::SubpassDependency>& dependencies)
	{
		if (hasDepthStencilAttachment)
		{
			auto& depthStencilAttachment = renderPassAttachments[colorAttachmentCount];

			if (PixelFormatInfo::GetContent(depthStencilAttachment.format) == PixelFormatContent::DepthStencil)
			{
				depthStencilAttachment.stencilLoadOp = depthStencilAttachment.loadOp;
				depthStencilAttachment.stencilStoreOp = depthStencilAttachment.storeOp;
			}
			else
			{
				depthStencilAttachment.stencilLoadOp = AttachmentLoadOp::Discard;
				depthStencilAttachment.stencilStoreOp = AttachmentStoreOp::Discard;
			}
		}

		struct SubpassInfo
		{
			bool hasColorWrite = false;
			bool hasDepthStencilRead = false;
			bool hasDepthStencilWrite = false;
			bool externalColorSynchronization = false;
			bool externalDepthSynchronization = false;
		};

		StackArray<SubpassInfo> subpassInfo = NazaraStackArray(SubpassInfo, subpasses.size());

		for (std::size_t attachmentIndex = 0; attachmentIndex < renderPassAttachments.size(); ++attachmentIndex)
		{
			bool used = false; //< has the attachment already been used in a previous subpass
			TextureLayout currentLayout = renderPassAttachments[attachmentIndex].initialLayout;

			auto FindColor = [&](std::size_t subpassIndex) -> RenderPass::AttachmentReference*
			{
				auto& subpassDesc = subpasses[subpassIndex];
				for (auto& colorReference : subpassDesc.colorAttachment)
				{
					if (colorReference.attachmentIndex == attachmentIndex)
						return &colorReference;
				}

				return nullptr;
			};

			auto FindDepthStencil = [&](std::size_t subpassIndex) -> RenderPass::AttachmentReference*
			{
				auto& subpassDesc = subpasses[subpassIndex];
				if (subpassDesc.depthStencilAttachment && subpassDesc.depthStencilAttachment->attachmentIndex == attachmentIndex)
					return &subpassDesc.depthStencilAttachment.value();

				return nullptr;
			};

			for (std::size_t subpassIndex = 0; subpassIndex < subpasses.size(); ++subpassIndex)
			{
				RenderPass::SubpassDescription& subpassDesc = subpasses[subpassIndex];

				RenderPass::AttachmentReference* colorAttachment = FindColor(subpassIndex);
				RenderPass::AttachmentReference* depthStencilAttachment = FindDepthStencil(subpassIndex);

				if (!colorAttachment && !depthStencilAttachment)
				{
					if (used)
						subpassDesc.preserveAttachments.push_back(attachmentIndex);

					continue;
				}

				if (colorAttachment)
				{
					subpassInfo[subpassIndex].hasColorWrite = true;

					currentLayout = colorAttachment->attachmentLayout;

					// If this subpass performs a layout change, color must be synchronized with external
					if (!used && renderPassAttachments[attachmentIndex].initialLayout != currentLayout)
						subpassInfo[subpassIndex].externalColorSynchronization = true;
				}
				else if (depthStencilAttachment)
				{
					switch (depthStencilAttachment->attachmentLayout)
					{
						case TextureLayout::DepthReadOnlyStencilReadWrite:
						case TextureLayout::DepthReadWriteStencilReadOnly:
						case TextureLayout::DepthStencilReadWrite:
							subpassInfo[subpassIndex].hasDepthStencilWrite = true;
							break;

						default:
							break;
					}

					subpassInfo[subpassIndex].hasDepthStencilRead = true;

					currentLayout = depthStencilAttachment->attachmentLayout;

					// If this subpass performs a layout change, depth must be synchronized with external
					if (!used && renderPassAttachments[attachmentIndex].initialLayout != currentLayout)
						subpassInfo[subpassIndex].externalDepthSynchronization = true;
				}

				used = true;
			}
		}

		// Handle external subpass dependencies
		for (std::size_t subpassIndex = 0; subpassIndex < subpasses.size(); ++subpassIndex)
		{
			const auto& sync = subpassInfo[subpassIndex];
			if (!sync.externalColorSynchronization && !sync.externalDepthSynchronization)
				continue;

			auto& subpassDependency = dependencies.emplace_back();
			subpassDependency.fromSubpassIndex = RenderPass::ExternalSubpassIndex;
			subpassDependency.toSubpassIndex = subpassIndex;
			subpassDependency.tilable = true;

			// TODO: Handle bottom of pipe?

			if (sync.externalColorSynchronization)
			{
				subpassDependency.fromStages |= PipelineStage::ColorOutput;
				subpassDependency.fromAccessFlags |= MemoryAccess::ColorWrite;

				subpassDependency.toStages |= PipelineStage::ColorOutput;
				subpassDependency.toAccessFlags |= MemoryAccess::ColorRead | MemoryAccess::ColorWrite;
			}

			if (sync.externalDepthSynchronization)
			{
				subpassDependency.fromStages |= PipelineStage::FragmentTestsLate;
				subpassDependency.fromAccessFlags |= MemoryAccess::DepthStencilWrite;

				subpassDependency.toStages |= PipelineStage::FragmentTestsEarly | PipelineStage::FragmentTestsLate;
				subpassDependency.toAccessFlags |= MemoryAccess::DepthStencilRead | MemoryAccess::DepthStencilWrite;
			}
		}

		// TODO: Handle self-dependencies

		// Handle pass to pass dependencies
		for (std::size_t subpassIndex = 1; subpassIndex < subpasses.size(); ++subpassIndex)
		{
			auto& subpassDependency = dependencies.emplace_back();
			subpassDependency.fromSubpassIndex = subpassIndex - 1;
			subpassDependency.toSubpassIndex = subpassIndex;
			subpassDependency.tilable = true;

			const auto& prevSync = subpassInfo[subpassDependency.fromSubpassIndex];
			const auto& sync = subpassInfo[subpassDependency.toSubpassIndex];

			// Previous pass flags

			if (prevSync.hasColorWrite)
			{
				subpassDependency.fromAccessFlags = MemoryAccess::ColorWrite;
				subpassDependency.fromStages = PipelineStage::ColorOutput;
			}

			if (prevSync.hasDepthStencilRead)
			{
				subpassDependency.fromStages |= PipelineStage::FragmentTestsEarly | PipelineStage::FragmentTestsLate;
				subpassDependency.fromAccessFlags |= MemoryAccess::DepthStencilRead;
			}

			if (prevSync.hasDepthStencilWrite)
			{
				subpassDependency.fromStages |= PipelineStage::FragmentTestsEarly | PipelineStage::FragmentTestsLate;
				subpassDependency.fromAccessFlags |= MemoryAccess::DepthStencilWrite;
			}

			// Current pass flags

			if (sync.hasColorWrite)
			{
				subpassDependency.toStages = PipelineStage::ColorOutput;
				subpassDependency.toAccessFlags = MemoryAccess::ColorRead | MemoryAccess::ColorWrite;
			}

			if (sync.hasDepthStencilRead)
			{
				subpassDependency.toStages |= PipelineStage::FragmentTestsEarly | PipelineStage::FragmentTestsLate;
				subpassDependency.toAccessFlags |= MemoryAccess::DepthStencilRead;
			}

			if (sync.hasDepthStencilWrite)
			{
				subpassDependency.toStages |= PipelineStage::FragmentTestsEarly | PipelineStage::FragmentTestsLate;
				subpassDependency.toAccessFlags |= MemoryAccess::DepthStencilRead | MemoryAccess::DepthStencilWrite;
			}

			// TODO: Handle InputAttachment
		}
	}

	void FrameGraph::BuildPhysicalPasses()
	{
		const RenderPassCache& renderPassCache = Graphics::Instance()->GetRenderPassCache();

		std::vector<TextureLayout> textureLayouts(m_pending.textures.size(), TextureLayout::Undefined);

		// Per-pass data (reuse memory)
		std::optional<std::size_t> depthStencilAttachmentIndex;
		std::size_t depthStencilAttachmentId;
		std::unordered_map<std::size_t /*textureId*/, std::size_t /*attachmentIndex*/> usedTextureAttachments;
		std::vector<RenderPass::Attachment> renderPassAttachments;
		std::vector<RenderPass::SubpassDescription> subpassesDesc;
		std::vector<RenderPass::SubpassDependency> subpassesDeps;

		auto RegisterColorInputRead = [&](const FramePass::Input& input)
		{
			std::size_t textureId = Retrieve(m_pending.attachmentToTextures, ResolveAttachmentIndex(input.attachmentId));
			if (textureId == InvalidTextureIndex)
				return;

			// For texture view, use the parent texture layout
			if (m_pending.textures[textureId].viewData)
				textureId = m_pending.textures[textureId].viewData->parentTextureId;

			TextureLayout& textureLayout = textureLayouts[textureId];
			if (!input.assumedLayout)
			{
				NazaraAssertMsg(textureLayouts[textureId] != TextureLayout::Undefined, "Texture has Undefined layout, this can happen if you're trying to read a texture that wasn't read to");
				textureLayout = input.layout;
			}
			else
				textureLayout = *input.assumedLayout;
		};

		auto RegisterOutput = [&](const FramePass::Output& output, bool shouldLoad)
		{
			std::size_t textureId = Retrieve(m_pending.attachmentToTextures, ResolveAttachmentIndex(output.attachmentId));
			if (textureId == InvalidTextureIndex)
				return InvalidAttachmentIndex;

			// For texture view, use the parent texture layout
			if (m_pending.textures[textureId].viewData)
				textureId = m_pending.textures[textureId].viewData->parentTextureId;

			TextureLayout initialLayout = textureLayouts[textureId];
			textureLayouts[textureId] = output.layout;

			if (!output.textureUsageFlags.Test(TextureUsage::ColorAttachment))
				return InvalidAttachmentIndex;

			auto it = usedTextureAttachments.find(textureId);
			if (it != usedTextureAttachments.end())
				return it->second;

			std::size_t attachmentIndex = renderPassAttachments.size();
			auto& attachment = renderPassAttachments.emplace_back();
			attachment.format = m_pending.textures[textureId].format;
			attachment.initialLayout = initialLayout;
			attachment.storeOp = AttachmentStoreOp::Store;
			attachment.stencilLoadOp = AttachmentLoadOp::Discard;
			attachment.stencilStoreOp = AttachmentStoreOp::Discard;

			if (output.clearColor)
				attachment.loadOp = AttachmentLoadOp::Clear;
			else if (shouldLoad)
				attachment.loadOp = AttachmentLoadOp::Load;
			else
				attachment.loadOp = AttachmentLoadOp::Discard;

			usedTextureAttachments.emplace(textureId, attachmentIndex);
			return attachmentIndex;
		};

		auto RegisterDepthStencil = [&](std::size_t attachmentId, TextureLayout textureLayout, bool* first) -> RenderPass::Attachment*
		{
			if (depthStencilAttachmentIndex)
			{
				assert(depthStencilAttachmentId == attachmentId);
				*first = false;

				return &renderPassAttachments[depthStencilAttachmentIndex.value()];
			}

			*first = true;

			std::size_t textureId = Retrieve(m_pending.attachmentToTextures, ResolveAttachmentIndex(attachmentId));
			if (textureId == InvalidTextureIndex)
				return nullptr;

			// For texture view, use the parent texture layout
			if (m_pending.textures[textureId].viewData)
				textureId = m_pending.textures[textureId].viewData->parentTextureId;

			textureLayouts[textureId] = textureLayout;

			depthStencilAttachmentId = attachmentId;
			depthStencilAttachmentIndex = renderPassAttachments.size();

			usedTextureAttachments.emplace(textureId, *depthStencilAttachmentIndex);

			auto& depthStencilAttachment = renderPassAttachments.emplace_back();
			depthStencilAttachment.format = m_pending.textures[textureId].format;
			depthStencilAttachment.initialLayout = textureLayout;

			return &depthStencilAttachment;
		};

		std::size_t physicalPassIndex = 0;
		for (auto& physicalPass : m_pending.physicalPasses)
		{
			depthStencilAttachmentIndex = std::nullopt;
			usedTextureAttachments.clear();
			renderPassAttachments.clear();
			subpassesDesc.clear();
			subpassesDeps.clear();

			std::size_t subpassIndex = 0;

			std::vector<RenderPass::AttachmentReference> colorAttachments;
			for (auto& subpass : physicalPass.passes)
			{
				const FramePass& framePass = m_framePasses[subpass.passIndex];
				const auto& subpassInputs = framePass.GetInputs();
				const auto& subpassOutputs = framePass.GetOutputs();

				colorAttachments.reserve(subpassOutputs.size());

				for (const auto& input : subpassInputs)
				{
					if (input.doesRead)
						RegisterColorInputRead(input);
				}

				for (const auto& output : subpassOutputs)
				{
					bool shouldLoad = false;

					// load content if read-write
					if (HasAttachment(subpassInputs, output.attachmentId))
						shouldLoad = true;

					std::size_t attachmentIndex = RegisterOutput(output, shouldLoad);
					if (attachmentIndex != InvalidAttachmentIndex)
					{
						colorAttachments.push_back({
							attachmentIndex,
							output.layout
						});
					}
				}
			}

			std::size_t colorAttachmentCount = renderPassAttachments.size();

			std::optional<RenderPass::AttachmentReference> depthStencilAttachment;
			for (auto& subpass : physicalPass.passes)
			{
				const FramePass& framePass = m_framePasses[subpass.passIndex];
				std::size_t dsInputAttachment = FramePass::InvalidAttachmentId;
				if (const auto& dsInput = framePass.GetDepthStencilInput())
					dsInputAttachment = dsInput->attachmentId;

				std::size_t dsOutputAttachement = framePass.GetDepthStencilOutput();

				if (dsInputAttachment != FramePass::InvalidAttachmentId && ResolveAttachmentIndex(dsOutputAttachement) == ResolveAttachmentIndex(dsInputAttachment))
				{
					TextureLayout layout = GetWriteDepthStencilLayout(dsOutputAttachement);

					// DS input/output
					bool first;
					RenderPass::Attachment* dsAttachment = RegisterDepthStencil(dsInputAttachment, layout, &first);
					if (dsAttachment)
					{
						if (first)
						{
							dsAttachment->loadOp = AttachmentLoadOp::Load;
							dsAttachment->storeOp = AttachmentStoreOp::Store;
						}

						depthStencilAttachment = RenderPass::AttachmentReference{
							depthStencilAttachmentIndex.value(),
							layout
						};
					}
				}
				else if (dsInputAttachment != FramePass::InvalidAttachmentId)
				{
					// DS input-only
					bool first;
					RenderPass::Attachment* dsAttachment = RegisterDepthStencil(dsInputAttachment, TextureLayout::DepthStencilReadOnly, &first);
					if (dsAttachment)
					{
						if (first)
						{
							bool canDiscard = true;

							// Check if a future pass reads from the DS buffer or if we can discard it after this pass
							if (auto readIt = m_pending.attachmentReadList.find(dsInputAttachment); readIt != m_pending.attachmentReadList.end())
							{
								for (std::size_t passIndex : readIt->second)
								{
									auto it = m_pending.passIdToPhysicalPassIndex.find(passIndex);
									if (it == m_pending.passIdToPhysicalPassIndex.end())
										continue; //< pass may have been discarded

									std::size_t readPhysicalPassIndex = it->second;
									if (readPhysicalPassIndex > physicalPassIndex) //< Read in a future pass?
									{
										// Yes, store it
										canDiscard = false;
										break;
									}
								}
							}

							dsAttachment->storeOp = (canDiscard) ? AttachmentStoreOp::Discard : AttachmentStoreOp::Store;
						}

						depthStencilAttachment = RenderPass::AttachmentReference{
							depthStencilAttachmentIndex.value(),
							TextureLayout::DepthStencilReadOnly
						};
					}
				}
				else if (dsOutputAttachement != FramePass::InvalidAttachmentId)
				{
					TextureLayout layout = GetWriteDepthStencilLayout(dsOutputAttachement);

					// DS output-only
					bool first;
					RenderPass::Attachment* dsAttachment = RegisterDepthStencil(dsOutputAttachement, layout, &first);
					if (dsAttachment)
					{
						if (first)
						{
							dsAttachment->initialLayout = TextureLayout::Undefined; //< Don't care about initial layout
							dsAttachment->loadOp = (framePass.GetDepthStencilClear()) ? AttachmentLoadOp::Clear : AttachmentLoadOp::Discard;
							dsAttachment->storeOp = AttachmentStoreOp::Store;
						}

						depthStencilAttachment = RenderPass::AttachmentReference{
							depthStencilAttachmentIndex.value(),
							layout
						};
					}
				}

				subpassesDesc.push_back({
					std::move(colorAttachments),
					{},
					{},
					std::move(depthStencilAttachment)
				});

				subpassIndex++;
			}

			// Assign final layout (TODO: Use this to perform layouts useful for future passes?)
			for (const auto& [textureId, attachmentIndex] : usedTextureAttachments)
			{
				auto& attachment = renderPassAttachments[attachmentIndex];
				attachment.finalLayout = textureLayouts[textureId];
			}

			BuildPhysicalPassDependencies(colorAttachmentCount, depthStencilAttachmentIndex.has_value(), renderPassAttachments, subpassesDesc, subpassesDeps);

			if (!renderPassAttachments.empty())
				m_pending.renderPasses.push_back(renderPassCache.Get(renderPassAttachments, subpassesDesc, subpassesDeps));
			else
				m_pending.renderPasses.push_back(nullptr);

			physicalPassIndex++;
		}
	}

	void FrameGraph::BuildReadWriteList()
	{
		for (std::size_t passIndex = 0; passIndex < m_framePasses.size(); ++passIndex)
		{
			const FramePass& framePass = m_framePasses[passIndex];

			for (const auto& input : framePass.GetInputs())
				UniquePushBack(m_pending.attachmentReadList[input.attachmentId], passIndex);

			if (const auto& depthStencilInput = framePass.GetDepthStencilInput())
				UniquePushBack(m_pending.attachmentReadList[depthStencilInput->attachmentId], passIndex);

			for (const auto& output : framePass.GetOutputs())
				UniquePushBack(m_pending.attachmentWriteList[output.attachmentId], passIndex);

			if (std::size_t depthStencilId = framePass.GetDepthStencilOutput(); depthStencilId != FramePass::InvalidAttachmentId)
				UniquePushBack(m_pending.attachmentWriteList[depthStencilId], passIndex);
		}
	}

	bool FrameGraph::HasAttachment(const std::vector<FramePass::Input>& inputs, std::size_t attachmentIndex) const
	{
		attachmentIndex = ResolveAttachmentIndex(attachmentIndex);

		for (const auto& input : inputs)
		{
			if (ResolveAttachmentIndex(input.attachmentId) == attachmentIndex)
				return true;
		}

		return false;
	}

	void FrameGraph::RegisterPassInput(std::size_t passIndex, std::size_t attachmentIndex)
	{
		auto it = m_pending.attachmentWriteList.find(attachmentIndex);
		if (it != m_pending.attachmentWriteList.end())
		{
			const PassList& dependencyPassList = it->second;
			for (std::size_t dependencyPass : dependencyPassList)
			{
				if (dependencyPass != passIndex)
					TraverseGraph(dependencyPass);
			}
		}
	}

	std::size_t FrameGraph::RegisterTexture(std::size_t attachmentIndex)
	{
		if (auto it = m_pending.attachmentToTextures.find(attachmentIndex); it != m_pending.attachmentToTextures.end())
			return it->second;

		auto InsertTexture = [this](ImageType imageType, std::size_t attachmentIndex, const FramePassAttachment& attachmentData, std::size_t& textureId) -> FrameGraphTextureData&
		{
			textureId = m_pending.textures.size();
			m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

			FrameGraphTextureData& data = m_pending.textures.emplace_back();
			data.type = imageType;
			data.name = attachmentData.name;
			data.format = attachmentData.format;
			data.width = attachmentData.width;
			data.height = attachmentData.height;
			data.size = attachmentData.size;
			data.layerCount = 1;
			data.usage = attachmentData.additionalUsages;
			data.viewerIndex = attachmentData.viewerIndex;
			data.canReuse = true;

			return data;
		};

		auto CheckExternalTexture = [this](std::size_t attachmentIndex, FrameGraphTextureData& data)
		{
			attachmentIndex = ResolveAttachmentIndex(attachmentIndex);

			// Check if texture is bound to an external texture
			if (auto externalIt = m_externalTextures.find(attachmentIndex); externalIt != m_externalTextures.end())
			{
				if (data.viewData)
					throw std::runtime_error("texture views cannot be bound to external textures");

				const std::shared_ptr<Texture>& externalTexture = externalIt->second;
				const TextureInfo& textureInfo = externalTexture->GetTextureInfo();

				// Check that texture settings match
				if (textureInfo.type != data.type)
					throw std::runtime_error("external texture type doesn't match attachment type");

				if (textureInfo.layerCount != data.layerCount)
					throw std::runtime_error("external texture layer count doesn't match attachment type");

				if (textureInfo.pixelFormat != data.format)
					throw std::runtime_error("external texture format doesn't match attachment type");

				data.canReuse = false;
				data.externalTexture = externalTexture;
				data.size = FramePassAttachmentSize::Fixed;
				data.width = textureInfo.width;
				data.height = textureInfo.height;
			}
		};

		return std::visit([&](auto&& arg) -> std::size_t
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, FramePassAttachment>)
			{
				const FramePassAttachment& attachmentData = arg;

				// Fetch from reuse pool if possible
				for (auto it = m_pending.texture2DPool.begin(); it != m_pending.texture2DPool.end(); ++it)
				{
					std::size_t textureId = *it;

					FrameGraphTextureData& data = m_pending.textures[textureId];
					assert(data.type == ImageType::E2D);

					if (data.format != attachmentData.format ||
						data.width  != attachmentData.width  ||
						data.height != attachmentData.height ||
						data.size   != attachmentData.size)
						continue;

					if (data.size == FramePassAttachmentSize::ViewerTargetFactor && data.viewerIndex != attachmentData.viewerIndex)
						continue;

					m_pending.texture2DPool.erase(it);
					m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

					if (!attachmentData.name.empty() && data.name != attachmentData.name)
						data.name += " / " + attachmentData.name;

					return textureId;
				}

				std::size_t textureId;
				FrameGraphTextureData& data = InsertTexture(ImageType::E2D, attachmentIndex, attachmentData, textureId);
				data.layerCount = 1;

				CheckExternalTexture(attachmentIndex, data);

				// Final outputs cannot be reused
				if (std::find(m_graphOutputs.begin(), m_graphOutputs.end(), attachmentIndex) != m_graphOutputs.end())
					data.canReuse = false;

				return textureId;
			}
			else if constexpr (std::is_same_v<T, AttachmentArray>)
			{
				const AttachmentArray& attachmentData = arg;

				// Fetch from reuse pool if possible
				for (auto it = m_pending.texture2DArrayPool.begin(); it != m_pending.texture2DArrayPool.end(); ++it)
				{
					std::size_t textureId = *it;

					FrameGraphTextureData& data = m_pending.textures[textureId];
					assert(data.type == ImageType::E2D_Array);

					if (data.format != attachmentData.format ||
						data.width != attachmentData.width ||
						data.height != attachmentData.height ||
						data.size != attachmentData.size ||
						data.layerCount != attachmentData.layerCount)
						continue;

					if (data.size == FramePassAttachmentSize::ViewerTargetFactor && data.viewerIndex != attachmentData.viewerIndex)
						continue;

					m_pending.texture2DArrayPool.erase(it);
					m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

					if (!attachmentData.name.empty() && data.name != attachmentData.name)
						data.name += " / " + attachmentData.name;

					return textureId;
				}

				std::size_t textureId;
				FrameGraphTextureData& data = InsertTexture(ImageType::E2D_Array, attachmentIndex, attachmentData, textureId);
				data.layerCount = attachmentData.layerCount;

				CheckExternalTexture(attachmentIndex, data);

				// Final outputs cannot be reused
				if (std::find(m_graphOutputs.begin(), m_graphOutputs.end(), attachmentIndex) != m_graphOutputs.end())
					data.canReuse = false;

				return textureId;
			}
			else if constexpr (std::is_same_v<T, AttachmentCube>)
			{
				const AttachmentCube& attachmentData = arg;

				// Fetch from reuse pool if possible
				for (auto it = m_pending.textureCubePool.begin(); it != m_pending.textureCubePool.end(); ++it)
				{
					std::size_t textureId = *it;

					FrameGraphTextureData& data = m_pending.textures[textureId];
					assert(data.type == ImageType::Cubemap);

					if (data.format != attachmentData.format ||
						data.width  != attachmentData.width  ||
						data.height != attachmentData.height ||
						data.size   != attachmentData.size)
						continue;

					if (data.size == FramePassAttachmentSize::ViewerTargetFactor && data.viewerIndex != attachmentData.viewerIndex)
						continue;

					m_pending.textureCubePool.erase(it);
					m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

					if (!attachmentData.name.empty() && data.name != attachmentData.name)
						data.name += " / " + attachmentData.name;

					return textureId;
				}

				std::size_t textureId;
				FrameGraphTextureData& data = InsertTexture(ImageType::Cubemap, attachmentIndex, attachmentData, textureId);
				data.layerCount = 1;

				CheckExternalTexture(attachmentIndex, data);

				// Final outputs cannot be reused
				if (std::find(m_graphOutputs.begin(), m_graphOutputs.end(), attachmentIndex) != m_graphOutputs.end())
					data.canReuse = false;

				return textureId;
			}
			else if constexpr (std::is_same_v<T, AttachmentLayer>)
			{
				const AttachmentLayer& texLayer = arg;

				// TODO: Reuse texture views from pool?

				std::size_t parentTextureId = RegisterTexture(texLayer.attachmentId);

				std::size_t textureId = m_pending.textures.size();
				m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

				FrameGraphTextureData& data = m_pending.textures.emplace_back();
				const FrameGraphTextureData& parentTexture = m_pending.textures[parentTextureId];

				data.type = ImageType::E2D;
				data.format = parentTexture.format;
				data.width = parentTexture.width;
				data.height = parentTexture.height;
				data.size = parentTexture.size;
				data.viewData = {
					parentTextureId,
					texLayer.layerIndex,
					1
				};
				data.viewerIndex = parentTexture.viewerIndex;

				CheckExternalTexture(attachmentIndex, data);

				return textureId;
			}
			else if constexpr (std::is_same_v<T, AttachmentProxy>)
			{
				const AttachmentProxy& proxy = arg;

				std::size_t textureId = RegisterTexture(proxy.attachmentId);
				m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

				if (std::find(m_graphOutputs.begin(), m_graphOutputs.end(), attachmentIndex) != m_graphOutputs.end())
					m_pending.textures[textureId].canReuse = false;

				return textureId;
			}
			else if constexpr (std::is_same_v<T, AttachmentView>)
			{
				const AttachmentView& view = arg;

				// TODO: Reuse texture views from pool?

				std::size_t parentTextureId = RegisterTexture(view.attachmentId);

				std::size_t textureId = m_pending.textures.size();
				m_pending.attachmentToTextures.emplace(attachmentIndex, textureId);

				FrameGraphTextureData& data = m_pending.textures.emplace_back();
				const FrameGraphTextureData& parentTexture = m_pending.textures[parentTextureId];

				data.type = parentTexture.type;
				data.format = view.format != PixelFormat::Undefined ? view.format : parentTexture.format;
				data.width = parentTexture.width;
				data.height = parentTexture.height;
				data.size = parentTexture.size;
				data.viewData = {
					parentTextureId,
					0,
					parentTexture.layerCount,
					view.planeFlags
				};
				data.viewerIndex = parentTexture.viewerIndex;

				CheckExternalTexture(attachmentIndex, data);

				return textureId;
			}
			else if constexpr (std::is_same_v<T, DummyAttachment>)
			{
				m_pending.attachmentToTextures.emplace(attachmentIndex, InvalidTextureIndex);
				return InvalidTextureIndex;
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, m_attachments[attachmentIndex]);
	}

	void FrameGraph::RemoveDuplicatePasses()
	{
		// A way to remove duplicates from a std::vector without sorting it
		Bitset<> seen(m_framePasses.size(), false);

		auto itRead = m_pending.passList.begin();
		auto itWrite = m_pending.passList.begin();

		while (itRead != m_pending.passList.end())
		{
			std::size_t passIndex = *itRead;
			if (!seen[passIndex])
			{
				seen[passIndex] = true;

				if (itRead != itWrite)
					*itWrite++ = passIndex;
				else
					++itWrite;
			}

			++itRead;
		}

		m_pending.passList.erase(itWrite, m_pending.passList.end());
	}

	std::size_t FrameGraph::ResolveAttachmentIndex(std::size_t attachmentIndex) const
	{
		assert(attachmentIndex < m_attachments.size());

		while (const AttachmentProxy* proxy = std::get_if<AttachmentProxy>(&m_attachments[attachmentIndex]))
			attachmentIndex = proxy->attachmentId;

		return attachmentIndex;
	}

	void FrameGraph::ReorderPasses()
	{
		/* TODO */
	}

	void FrameGraph::TraverseGraph(std::size_t passIndex)
	{
		m_pending.passList.push_back(passIndex);

		const FramePass& framePass = m_framePasses[passIndex];
		for (const auto& input : framePass.GetInputs())
			RegisterPassInput(passIndex, input.attachmentId);

		if (const auto& depthStencilInput = framePass.GetDepthStencilInput())
			RegisterPassInput(passIndex, depthStencilInput->attachmentId);
	}
}
