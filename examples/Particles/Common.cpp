#include "Common.hpp"
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/Systems/RenderSystem.hpp>

ParticleDemo::ParticleDemo(const Nz::String& name, const ExampleShared& exampleShared) :
m_shared(exampleShared),
m_index(s_demoIndex++),
m_name(name)
{
}

void ParticleDemo::Enter(Ndk::StateMachine& /*fsm*/)
{
	m_shared.demoName->Update(Nz::SimpleTextDrawer::Draw(Nz::String::Number(m_index+1) + " - " + m_name, 48));
	m_fpsCounter = 0;
	m_updateClock.Restart();

	Ndk::RenderSystem& renderSystem2D = m_shared.world2D->GetSystem<Ndk::RenderSystem>();
	Ndk::RenderSystem& renderSystem3D = m_shared.world3D->GetSystem<Ndk::RenderSystem>();
	m_oldBackground2D = renderSystem2D.GetDefaultBackground();
	m_oldBackground3D = renderSystem3D.GetDefaultBackground();
}

void ParticleDemo::Leave(Ndk::StateMachine& /*fsm*/)
{
	m_shared.world2D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(m_oldBackground2D);
	m_shared.world3D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(m_oldBackground3D);

	m_entities.clear();
	m_particleGroups.clear();
}

bool ParticleDemo::Update(Ndk::StateMachine& /*fsm*/, float elapsedTime)
{
	m_fpsCounter++;
	if (m_updateClock.GetMilliseconds() > 1000)
	{
		m_updateClock.Restart();

		m_shared.fpsCount->Update(Nz::SimpleTextDrawer::Draw(Nz::String::Number(m_fpsCounter) + " FPS", 24));
		m_fpsCounter = 0;

		unsigned int particleCount = 0;
		for (const Ndk::EntityHandle& entity : m_particleGroups)
		{
			const Ndk::ParticleGroupComponent& group = entity->GetComponent<Ndk::ParticleGroupComponent>();
			particleCount += group.GetParticleCount();
		}

		m_shared.particleCount->Update(Nz::SimpleTextDrawer::Draw(Nz::String::Number(particleCount) + " particles", 36));
	}

	return true;
}

void ParticleDemo::RegisterEntity(const Ndk::EntityHandle& entity)
{
	m_entities.emplace_back(entity);
}

void ParticleDemo::RegisterParticleGroup(const Ndk::EntityHandle& entity)
{
	NazaraAssert(entity->HasComponent<Ndk::ParticleGroupComponent>(), "Must have particle group component");

	m_particleGroups.emplace_back(entity);
}

std::size_t ParticleDemo::s_demoIndex = 0;
