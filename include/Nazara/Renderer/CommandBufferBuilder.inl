// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void CommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect)
	{
		return BeginRenderPass(framebuffer, renderPass, renderRect, nullptr, 0);
	}

	inline void CommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, std::initializer_list<ClearValues> clearValues)
	{
		return BeginRenderPass(framebuffer, renderPass, renderRect, clearValues.begin(), clearValues.size());
	}

	inline void CommandBufferBuilder::CopyBuffer(const RenderBufferView& from, const RenderBufferView& to)
	{
		return CopyBuffer(from, to, from.GetSize());
	}

	inline void CommandBufferBuilder::CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target)
	{
		return CopyBuffer(allocation, target, allocation.size);
	}
}
