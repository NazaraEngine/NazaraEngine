// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>

namespace Nz
{
	class AbstractBuffer;
	class Buffer;

	class NAZARA_RENDERER_API Renderer : public Module<Renderer>
	{
		friend Module;

		public:
			using Dependencies = TypeList<Platform>;

			Renderer();
			~Renderer();

			inline RendererImpl* GetRendererImpl();

		private:
			std::unique_ptr<RendererImpl> m_rendererImpl;
			DynLib m_rendererLib;

			static Renderer* s_instance;
	};
}

#include <Nazara/Renderer/Renderer.inl>

#endif // NAZARA_RENDERER_HPP
