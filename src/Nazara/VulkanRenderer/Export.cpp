// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Export.hpp>
#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderer.hpp>

#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS

extern "C"
{
	NAZARA_EXPORT Nz::RendererImpl* NazaraRenderer_Instantiate()
	{
		std::unique_ptr<Nz::VulkanRenderer> renderer = std::make_unique<Nz::VulkanRenderer>();
		return renderer.release();
	}
}

#endif
