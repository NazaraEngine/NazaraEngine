#pragma once

#ifndef NAZARA_EXAMPLES_PARTICLES_COMMON_HPP
#define NAZARA_EXAMPLES_PARTICLES_COMMON_HPP

#include <Nazara/Core/Clock.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/EntityOwner.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>
#include <memory>
#include <random>
#include <vector>

class ParticleDemo;

struct ExampleShared
{
	mutable std::mt19937 randomGen;
	std::vector<std::shared_ptr<ParticleDemo>> demos;
	Nz::RenderWindow* target;
	Nz::TextSpriteRef demoName;
	Nz::TextSpriteRef fpsCount;
	Nz::TextSpriteRef particleCount;
	Ndk::EntityHandle viewer2D;
	Ndk::EntityHandle viewer3D;
	Ndk::WorldHandle world2D;
	Ndk::WorldHandle world3D;
};

class ParticleDemo : public Ndk::State
{
	public:
		ParticleDemo(const Nz::String& name, const ExampleShared& exampleShared);
		~ParticleDemo() = default;

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;

		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	protected:
		const ExampleShared& m_shared;

		void RegisterEntity(const Ndk::EntityHandle& entity);
		void RegisterParticleGroup(const Ndk::EntityHandle& entity);

	private:
		std::size_t m_index;
		std::vector<Ndk::EntityOwner> m_entities;
		std::vector<Ndk::EntityOwner> m_particleGroups;
		Nz::BackgroundRef m_oldBackground2D;
		Nz::BackgroundRef m_oldBackground3D;
		Nz::Clock m_updateClock;
		Nz::String m_name;
		unsigned int m_fpsCounter;

		static std::size_t s_demoIndex;
};

#endif // NAZARA_EXAMPLES_PARTICLES_COMMON_HPP
