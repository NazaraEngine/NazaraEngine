#pragma once

#ifndef NAZARA_EXAMPLES_PARTICLES_SPACEBATTLE_HPP
#define NAZARA_EXAMPLES_PARTICLES_SPACEBATTLE_HPP

#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/ParticleStruct.hpp>
#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/EventHandler.hpp>
#include <NDK/Entity.hpp>
#include <NDK/State.hpp>
#include <vector>
#include "Common.hpp"

class SpacebattleExample : public ParticleDemo
{
	public:
		SpacebattleExample(ExampleShared& sharedData);
		~SpacebattleExample() = default;

		void Enter(Ndk::StateMachine& fsm) override;
		void Leave(Ndk::StateMachine& fsm) override;
		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

	private:
		void CreateSpaceShip();
		void CreateTurret();
		void OnMouseMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseMoveEvent& event);

		struct Turret
		{
			Nz::Model baseModel;
			Nz::Model cannonModel;
			Nz::Model cannonBaseModel;
			Nz::Model rotatingBaseModel;
			Ndk::EntityHandle baseEntity;
			Ndk::EntityHandle cannonAnchorEntity;
			Ndk::EntityHandle cannonEntity;
			Ndk::EntityHandle cannonBaseEntity;
			Ndk::EntityHandle rotatingBaseEntity;
		};

		Turret m_turret;
		float m_introTimer;
		float m_spaceshipSpawnCounter;
		float m_turretBaseRotation;
		float m_turretCannonBaseRotation;
		float m_turretShootTimer;
		Nz::Model m_spaceshipModel;
		Nz::Model m_spacestationModel;
		Nz::Music m_ambientMusic;
		Nz::ParticleDeclarationRef m_torpedoDeclaration;
		Nz::ParticleRendererRef m_laserBeamRenderer;
		Nz::Sound m_turretFireSound;
		Nz::Sound m_turretReloadSound;
		Nz::SkyboxBackground m_skybox;
		Ndk::EntityHandle m_introText;
		Ndk::EntityHandle m_spaceshipTemplate;
		Ndk::EntityHandle m_spacestationEntity;
		Ndk::ParticleGroupComponentHandle m_fireGroup;
		Ndk::ParticleGroupComponentHandle m_smokeGroup;
		Ndk::ParticleGroupComponentHandle m_torpedoGroup;

		NazaraSlot(Nz::EventHandler, OnMouseMoved, m_onMouseMoved);
};

#endif // NAZARA_EXAMPLES_PARTICLES_SPACEBATTLE_HPP
