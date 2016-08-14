// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEFUNCTIONRENDERER_HPP
#define NAZARA_PARTICLEFUNCTIONRENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <functional>

namespace Nz
{
	class ParticleFunctionRenderer;

	using ParticleFunctionRendererConstRef = ObjectRef<const ParticleFunctionRenderer>;
	using ParticleFunctionRendererRef = ObjectRef<ParticleFunctionRenderer>;

	class NAZARA_GRAPHICS_API ParticleFunctionRenderer : public ParticleRenderer
	{
		public:
			using Renderer = std::function<void(const ParticleGroup& /*group*/, const ParticleMapper& /*mapper*/, unsigned int /*startId*/, unsigned int /*endId*/, AbstractRenderQueue* /*renderQueue*/)>;

			inline ParticleFunctionRenderer(Renderer renderer);
			ParticleFunctionRenderer(const ParticleFunctionRenderer&) = default;
			~ParticleFunctionRenderer() = default;

			void Render(const ParticleGroup& group, const ParticleMapper& mapper, unsigned int startId, unsigned int endId, AbstractRenderQueue* renderQueue) override final;

			inline const Renderer& GetRenderer() const;

			inline void SetRenderer(Renderer renderer);

			template<typename... Args> static ParticleFunctionRendererRef New(Args&&... args);

		private:
			Renderer m_renderer;
	};
}

#include <Nazara/Graphics/ParticleFunctionRenderer.inl>

#endif // NAZARA_PARTICLEFUNCTIONRENDERER_HPP
