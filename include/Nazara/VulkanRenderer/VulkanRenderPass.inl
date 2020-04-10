// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VulkanRenderPass::VulkanRenderPass(Vk::RenderPass renderPass, std::initializer_list<PixelFormat> formats) :
	m_formats(std::begin(formats), std::end(formats)),
	m_renderPass(std::move(renderPass))
	{
	}

	inline PixelFormat VulkanRenderPass::GetAttachmentFormat(std::size_t attachmentIndex) const
	{
		return m_formats[attachmentIndex];
	}

	inline Vk::RenderPass& VulkanRenderPass::GetRenderPass()
	{
		return m_renderPass;
	}

	inline const Vk::RenderPass& VulkanRenderPass::GetRenderPass() const
	{
		return m_renderPass;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
