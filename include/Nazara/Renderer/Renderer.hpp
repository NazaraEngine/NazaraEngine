// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API Renderer
	{
		public:
			Renderer() = delete;
			~Renderer() = delete;
			
			static inline RendererImpl* GetRendererImpl();

			static bool Initialize();

			static inline bool IsInitialized();

			static void Uninitialize();

		private:
			static DynLib s_rendererLib;
			static std::unique_ptr<RendererImpl> s_rendererImpl;
			static unsigned int s_moduleReferenceCounter;
	};
}

#include <Nazara/Renderer/Renderer.inl>

#endif // NAZARA_RENDERER_HPP
