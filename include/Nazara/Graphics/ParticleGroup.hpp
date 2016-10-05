// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEGROUP_HPP
#define NAZARA_PARTICLEGROUP_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <functional>
#include <memory>
#include <set>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API ParticleGroup : public Renderable
	{
		public:
			ParticleGroup(unsigned int maxParticleCount, ParticleLayout layout);
			ParticleGroup(unsigned int maxParticleCount, ParticleDeclarationConstRef declaration);
			ParticleGroup(const ParticleGroup& emitter);
			~ParticleGroup();

			void AddController(ParticleControllerRef controller);
			void AddEmitter(ParticleEmitter* emitter);
			void AddGenerator(ParticleGeneratorRef generator);
			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const override;

			void ApplyControllers(ParticleMapper& mapper, unsigned int particleCount, float elapsedTime);

			void* CreateParticle();
			void* CreateParticles(unsigned int count);

			void* GenerateParticle();
			void* GenerateParticles(unsigned int count);

			const ParticleDeclarationConstRef& GetDeclaration() const;
			unsigned int GetMaxParticleCount() const;
			unsigned int GetParticleCount() const;
			unsigned int GetParticleSize() const;

			void KillParticle(unsigned int index);
			void KillParticles();

			void RemoveController(ParticleController* controller);
			void RemoveEmitter(ParticleEmitter* emitter);
			void RemoveGenerator(ParticleGenerator* generator);

			void SetRenderer(ParticleRenderer* renderer);

			void Update(float elapsedTime);
			void UpdateBoundingVolume(const Matrix4f& transformMatrix) override;

			ParticleGroup& operator=(const ParticleGroup& emitter);

			// Signals:
			NazaraSignal(OnParticleGroupRelease, const ParticleGroup* /*particleGroup*/);

		private:
			void MakeBoundingVolume() const override;
			void OnEmitterMove(ParticleEmitter* oldEmitter, ParticleEmitter* newEmitter);
			void OnEmitterRelease(const ParticleEmitter* emitter);
			void ResizeBuffer();

			struct EmitterEntry
			{
				NazaraSlot(ParticleEmitter, OnParticleEmitterMove, moveSlot);
				NazaraSlot(ParticleEmitter, OnParticleEmitterRelease, releaseSlot);

				ParticleEmitter* emitter;
			};

			std::set<unsigned int, std::greater<unsigned int>> m_dyingParticles;
			mutable std::vector<UInt8> m_buffer;
			std::vector<ParticleControllerRef> m_controllers;
			std::vector<EmitterEntry> m_emitters;
			std::vector<ParticleGeneratorRef> m_generators;
			ParticleDeclarationConstRef m_declaration;
			ParticleRendererRef m_renderer;
			bool m_processing;
			unsigned int m_maxParticleCount;
			unsigned int m_particleCount;
			unsigned int m_particleSize;
	};
}

#endif // NAZARA_PARTICLEGROUP_HPP
