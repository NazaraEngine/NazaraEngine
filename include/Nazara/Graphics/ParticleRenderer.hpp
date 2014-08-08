// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLERENDERER_HPP
#define NAZARA_PARTICLERENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>

class NzAbstractRenderQueue;
class NzParticleEmitter;
class NzParticleMapper;
class NzParticleRenderer;

using NzParticleRendererConstRef = NzResourceRef<const NzParticleRenderer>;
using NzParticleRendererRef = NzResourceRef<NzParticleRenderer>;

class NAZARA_API NzParticleRenderer : public NzResource
{
	public:
		NzParticleRenderer() = default;
		NzParticleRenderer(const NzParticleRenderer& renderer);
		virtual ~NzParticleRenderer();

		virtual void Render(const NzParticleEmitter& emitter, const NzParticleMapper& mapper, unsigned int offset, unsigned int particleCount, NzAbstractRenderQueue* renderQueue) = 0;
};

#endif // NAZARA_PARTICLERENDERER_HPP
