// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLESYSTEM_HPP
#define NAZARA_PARTICLESYSTEM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Graphics/ParticleController.hpp>
#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <Nazara/Graphics/ParticleRenderer.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <memory>
#include <set>
#include <vector>

class NAZARA_API NzParticleSystem : public NzSceneNode, NzUpdatable
{
	public:
		NzParticleSystem(unsigned int maxParticleCount, nzParticleLayout layout);
		NzParticleSystem(unsigned int maxParticleCount, const NzParticleDeclaration* declaration);
		NzParticleSystem(const NzParticleSystem& emitter);
		NzParticleSystem(NzParticleSystem&& emitter) = default;
		~NzParticleSystem();

		void AddController(NzParticleController* controller);
		void AddEmitter(NzParticleEmitter* emitter);
		void AddGenerator(NzParticleGenerator* generator);
		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const;

		void* CreateParticle();
		void* CreateParticles(unsigned int count);

		void EnableFixedStep(bool fixedStep);

		void* GenerateParticle();
		void* GenerateParticles(unsigned int count);

		const NzBoundingVolumef& GetBoundingVolume() const override;
		const NzParticleDeclaration* GetDeclaration() const;
		float GetFixedStepSize() const;
		unsigned int GetMaxParticleCount() const;
		unsigned int GetParticleCount() const;
		unsigned int GetParticleSize() const;
		nzSceneNodeType GetSceneNodeType() const override;

		bool IsDrawable() const;
		bool IsFixedStepEnabled() const;

		void KillParticle(unsigned int index);
		void KillParticles();

		void RemoveController(NzParticleController* controller);
		void RemoveEmitter(NzParticleEmitter* emitter);
		void RemoveGenerator(NzParticleGenerator* generator);

		void SetFixedStepSize(float stepSize);
		void SetRenderer(NzParticleRenderer* renderer);

		NzParticleSystem& operator=(const NzParticleSystem& emitter);
		NzParticleSystem& operator=(NzParticleSystem&& emitter) = default;

	private:
		void GenerateAABB() const;
		void Register() override;
		void ResizeBuffer();
		void Unregister() override;
		void UpdateBoundingVolume() const;
		void Update() override;

		std::set<unsigned int, std::greater<unsigned int>> m_dyingParticles;
		mutable std::vector<nzUInt8> m_buffer;
		std::vector<NzParticleControllerRef> m_controllers;
		std::vector<NzParticleEmitter*> m_emitters;
		std::vector<NzParticleGeneratorRef> m_generators;
		mutable NzBoundingVolumef m_boundingVolume;
		NzParticleDeclarationConstRef m_declaration;
		NzParticleRendererRef m_renderer;
		mutable bool m_boundingVolumeUpdated;
		bool m_fixedStepEnabled;
		bool m_processing;
		float m_stepAccumulator;
		float m_stepSize;
		unsigned int m_maxParticleCount;
		unsigned int m_particleCount;
		unsigned int m_particleSize;
};

#endif // NAZARA_PARTICLESYSTEM_HPP
