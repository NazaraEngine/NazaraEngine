// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void GpuCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const GpuRenderPass& renderPass, const Recti& renderRect)
	{
		return BeginRenderPass(framebuffer, renderPass, renderRect, nullptr, 0);
	}

	inline void GpuCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const GpuRenderPass& renderPass, const Recti& renderRect, std::initializer_list<ClearValues> clearValues)
	{
		return BeginRenderPass(framebuffer, renderPass, renderRect, clearValues.begin(), clearValues.size());
	}

	inline void GpuCommandBufferBuilder::BufferBarrier(const BufferBarrierInfo& barrierInfo)
	{
		return PipelineBarrier({}, std::span(&barrierInfo, 1), {});
	}

	inline void GpuCommandBufferBuilder::CopyBuffer(const GpuBufferView& from, const GpuBufferView& to)
	{
		return CopyBuffer(from, to, from.GetSize());
	}

	inline void GpuCommandBufferBuilder::CopyBuffer(const GpuUploadPool::Allocation& allocation, const GpuBufferView& target)
	{
		return CopyBuffer(allocation, target, allocation.size);
	}

	inline void GpuCommandBufferBuilder::MemoryBarrier(const MemoryBarrierInfo& barrierInfo)
	{
		return PipelineBarrier(std::span(&barrierInfo, 1), {}, {});
	}

	inline void GpuCommandBufferBuilder::TextureBarrier(const TextureBarrierInfo& barrierInfo)
	{
		return PipelineBarrier({}, {}, std::span(&barrierInfo, 1));
	}
}
