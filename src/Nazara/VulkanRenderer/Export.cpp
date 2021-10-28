// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>

extern "C"
{
	NAZARA_EXPORT Nz::RendererImpl* NazaraRenderer_Instantiate()
	{
		std::unique_ptr<Nz::VulkanRenderer> renderer = std::make_unique<Nz::VulkanRenderer>();
		return renderer.release();
	}
}
