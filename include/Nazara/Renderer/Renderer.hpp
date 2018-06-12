// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class AbstractBuffer;
	class Buffer;

	class NAZARA_RENDERER_API Renderer
	{
		public:
			Renderer() = delete;
			~Renderer() = delete;

			static inline RendererImpl* GetRendererImpl();

			static bool Initialize();

			static inline bool IsInitialized();

			static inline void SetParameters(const ParameterList& parameters);

			static void Uninitialize();

		private:
			static AbstractBuffer* CreateHardwareBufferImpl(Buffer* parent, BufferType type);

			static std::unique_ptr<RendererImpl> s_rendererImpl;
			static DynLib s_rendererLib;
			static ParameterList s_initializationParameters;
			static unsigned int s_moduleReferenceCounter;
	};
}

#include <Nazara/Renderer/Renderer.inl>

#endif // NAZARA_RENDERER_HPP
