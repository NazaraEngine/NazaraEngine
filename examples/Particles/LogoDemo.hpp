#pragma once

#ifndef NAZARA_EXAMPLES_PARTICLES_LOGO_HPP
#define NAZARA_EXAMPLES_PARTICLES_LOGO_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Graphics/ParticleStruct.hpp>
#include <NDK/State.hpp>
#include <vector>
#include "Common.hpp"

class LogoExample : public ParticleDemo
{
	public:
		LogoExample(ExampleShared& sharedData);
		~LogoExample() = default;

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void ResetParticles(float elapsed);
	
		struct PixelData
		{
			Nz::Vector2ui pos;
			Nz::Color color;
		};

		std::vector<PixelData> m_pixels;
		Nz::BackgroundRef m_oldBackground;
		Nz::ParticleStruct_Sprite* m_particles;
		Nz::Image m_logo;
		Nz::ParticleControllerRef m_controller;
		Nz::ParticleRendererRef m_renderer;
		float m_accumulator;
		float m_totalAccumulator;
};

#endif // NAZARA_EXAMPLES_PARTICLES_LOGO_HPP
