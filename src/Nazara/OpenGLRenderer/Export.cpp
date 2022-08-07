// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>

#ifndef NAZARA_RENDERER_EMBEDDEDBACKENDS

extern "C"
{
	NAZARA_EXPORT Nz::RendererImpl* NazaraRenderer_Instantiate()
	{
		std::unique_ptr<Nz::OpenGLRenderer> renderer = std::make_unique<Nz::OpenGLRenderer>();
		return renderer.release();
	}
}

#endif
