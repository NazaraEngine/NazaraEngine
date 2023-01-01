// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	void RenderFrame::Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags)
	{
		if (!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		return m_image->Execute(callback, queueTypeFlags);
	}

	UploadPool& RenderFrame::GetUploadPool()
	{
		if (!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		return m_image->GetUploadPool();
	}

	void RenderFrame::Present()
	{
		if (!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->Present();
		m_image = nullptr;
	}

	void RenderFrame::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		if (!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->SubmitCommandBuffer(commandBuffer, queueTypeFlags);
	}
}
