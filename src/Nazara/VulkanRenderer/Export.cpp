// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
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
