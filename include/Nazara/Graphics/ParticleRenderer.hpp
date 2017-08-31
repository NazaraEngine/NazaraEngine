// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLERENDERER_HPP
#define NAZARA_PARTICLERENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class AbstractRenderQueue;
	class ParticleMapper;
	class ParticleRenderer;
	class ParticleGroup;

	using ParticleRendererConstRef = ObjectRef<const ParticleRenderer>;
	using ParticleRendererLibrary = ObjectLibrary<ParticleRenderer>;
	using ParticleRendererRef = ObjectRef<ParticleRenderer>;

	class NAZARA_GRAPHICS_API ParticleRenderer : public RefCounted
	{
		friend ParticleRendererLibrary;
		friend class Graphics;

		public:
			ParticleRenderer() = default;
			ParticleRenderer(const ParticleRenderer& renderer);
			virtual ~ParticleRenderer();

			virtual void Render(const ParticleGroup& system, const ParticleMapper& mapper, unsigned int startId, unsigned int endId, AbstractRenderQueue* renderQueue) = 0;

			// Signals:
			NazaraSignal(OnParticleRendererRelease, const ParticleRenderer* /*particleRenderer*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			static ParticleRendererLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_PARTICLERENDERER_HPP
