// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLERENDERER_HPP
#define NAZARA_PARTICLERENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>

class NzAbstractRenderQueue;
class NzParticleMapper;
class NzParticleRenderer;
class NzParticleSystem;

using NzParticleRendererConstListener = NzObjectListenerWrapper<const NzParticleRenderer>;
using NzParticleRendererConstRef = NzObjectRef<const NzParticleRenderer>;
using NzParticleRendererLibrary = NzObjectLibrary<NzParticleRenderer>;
using NzParticleRendererListener = NzObjectListenerWrapper<NzParticleRenderer>;
using NzParticleRendererRef = NzObjectRef<NzParticleRenderer>;

class NAZARA_API NzParticleRenderer : public NzRefCounted
{
	friend NzParticleRendererLibrary;
	friend class NzGraphics;

	public:
		NzParticleRenderer() = default;
		NzParticleRenderer(const NzParticleRenderer& renderer);
		virtual ~NzParticleRenderer();

		virtual void Render(const NzParticleSystem& system, const NzParticleMapper& mapper, unsigned int startId, unsigned int endId, NzAbstractRenderQueue* renderQueue) = 0;

	private:
		static bool Initialize();
		static void Uninitialize();

		static NzParticleRendererLibrary::LibraryMap s_library;
};

#endif // NAZARA_PARTICLERENDERER_HPP
