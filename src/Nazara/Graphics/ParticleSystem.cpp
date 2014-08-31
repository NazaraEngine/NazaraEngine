// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleSystem.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <cstdlib>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzParticleSystem::NzParticleSystem(unsigned int maxParticleCount, nzParticleLayout layout) :
NzParticleSystem(maxParticleCount, NzParticleDeclaration::Get(layout))
{
}

NzParticleSystem::NzParticleSystem(unsigned int maxParticleCount, const NzParticleDeclaration* declaration) :
m_declaration(declaration),
m_boundingVolumeUpdated(false),
m_fixedStepEnabled(false),
m_processing(false),
m_stepAccumulator(0.f),
m_stepSize(1.f/60.f),
m_maxParticleCount(maxParticleCount),
m_particleCount(0)
{
	// En cas d'erreur, un constructeur ne peut que lancer une exception
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_particleSize = m_declaration->GetStride(); // La taille de chaque particule

	ResizeBuffer();
}

NzParticleSystem::NzParticleSystem(const NzParticleSystem& system) :
NzSceneNode(system),
m_controllers(system.m_controllers),
m_generators(system.m_generators),
m_boundingVolume(system.m_boundingVolume),
m_declaration(system.m_declaration),
m_renderer(system.m_renderer),
m_boundingVolumeUpdated(system.m_boundingVolumeUpdated),
m_fixedStepEnabled(system.m_fixedStepEnabled),
m_processing(false),
m_stepAccumulator(0.f),
m_stepSize(system.m_stepSize),
m_maxParticleCount(system.m_maxParticleCount),
m_particleCount(system.m_particleCount),
m_particleSize(system.m_particleSize)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	ResizeBuffer();

	// On ne copie que les particules vivantes
	std::memcpy(m_buffer.data(), system.m_buffer.data(), system.m_particleCount*m_particleSize);
}

NzParticleSystem::~NzParticleSystem() = default;

void NzParticleSystem::AddController(NzParticleController* controller)
{
	m_controllers.emplace_back(controller);
}

void NzParticleSystem::AddEmitter(NzParticleEmitter* emitter)
{
	m_emitters.emplace_back(emitter);
}

void NzParticleSystem::AddGenerator(NzParticleGenerator* generator)
{
	m_generators.emplace_back(generator);
}

void NzParticleSystem::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	///FIXME: Vérifier le renderer
	if (m_particleCount > 0)
	{
		NzParticleMapper mapper(m_buffer.data(), m_declaration);
		m_renderer->Render(*this, mapper, 0, m_particleCount-1, renderQueue);
	}
}

void* NzParticleSystem::CreateParticle()
{
	return CreateParticles(1);
}

void* NzParticleSystem::CreateParticles(unsigned int count)
{
	if (count == 0)
		return nullptr;

	if (m_particleCount+count > m_maxParticleCount)
		return nullptr;

	unsigned int particlesIndex = m_particleCount;
	m_particleCount += count;

	return &m_buffer[particlesIndex*m_particleSize];
}

void NzParticleSystem::EnableFixedStep(bool fixedStep)
{
	// On teste pour empêcher que cette méthode ne remette systématiquement le step accumulator à zéro
	if (m_fixedStepEnabled != fixedStep)
	{
		m_fixedStepEnabled = fixedStep;
		m_stepAccumulator = 0.f;
	}
}

void* NzParticleSystem::GenerateParticle()
{
	return GenerateParticles(1);
}

void* NzParticleSystem::GenerateParticles(unsigned int count)
{
	void* ptr = CreateParticles(count);
	if (!ptr)
		return nullptr;

	NzParticleMapper mapper(ptr, m_declaration);
	for (NzParticleGenerator* generator : m_generators)
		generator->Generate(*this, mapper, 0, m_particleCount-1);

	return ptr;
}

const NzBoundingVolumef& NzParticleSystem::GetBoundingVolume() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return m_boundingVolume;
}

const NzParticleDeclaration* NzParticleSystem::GetDeclaration() const
{
	return m_declaration;
}

float NzParticleSystem::GetFixedStepSize() const
{
	return m_stepSize;
}

unsigned int NzParticleSystem::GetMaxParticleCount() const
{
	return m_maxParticleCount;
}

unsigned int NzParticleSystem::GetParticleCount() const
{
	return m_particleCount;
}

unsigned int NzParticleSystem::GetParticleSize() const
{
	return m_particleSize;
}

nzSceneNodeType NzParticleSystem::GetSceneNodeType() const
{
	return nzSceneNodeType_ParticleEmitter;
}

bool NzParticleSystem::IsDrawable() const
{
	return true;
}

bool NzParticleSystem::IsFixedStepEnabled() const
{
	return m_fixedStepEnabled;
}

void NzParticleSystem::KillParticle(unsigned int index)
{
	///FIXME: Vérifier index

	if (m_processing)
	{
		// Le buffer est en train d'être modifié, nous ne pouvons pas réduire sa taille, on place alors la particule dans une liste de secours
		m_dyingParticles.insert(index);
		return;
	}

	// On déplace la dernière particule vivante à la place de celle-ci
	if (--m_particleCount > 0)
		std::memcpy(&m_buffer[index*m_particleSize], &m_buffer[m_particleCount*m_particleSize], m_particleSize);
}

void NzParticleSystem::KillParticles()
{
	m_particleCount = 0;
}

void NzParticleSystem::RemoveController(NzParticleController* controller)
{
	auto it = std::find(m_controllers.begin(), m_controllers.end(), controller);
	if (it != m_controllers.end())
		m_controllers.erase(it);
}

void NzParticleSystem::RemoveEmitter(NzParticleEmitter* emitter)
{
	auto it = std::find(m_emitters.begin(), m_emitters.end(), emitter);
	if (it != m_emitters.end())
		m_emitters.erase(it);
}

void NzParticleSystem::RemoveGenerator(NzParticleGenerator* generator)
{
	auto it = std::find(m_generators.begin(), m_generators.end(), generator);
	if (it != m_generators.end())
		m_generators.erase(it);
}

void NzParticleSystem::SetFixedStepSize(float stepSize)
{
	m_stepSize = stepSize;
}

void NzParticleSystem::SetRenderer(NzParticleRenderer* renderer)
{
	m_renderer = renderer;
}

NzParticleSystem& NzParticleSystem::operator=(const NzParticleSystem& system)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	NzSceneNode::operator=(system);

	m_boundingVolume = system.m_boundingVolume;
	m_boundingVolumeUpdated = system.m_boundingVolumeUpdated;
	m_controllers = system.m_controllers;
	m_declaration = system.m_declaration;
	m_fixedStepEnabled = system.m_fixedStepEnabled;
	m_generators = system.m_generators;
	m_maxParticleCount = system.m_maxParticleCount;
	m_particleCount = system.m_particleCount;
	m_particleSize = system.m_particleSize;
	m_renderer = system.m_renderer;
	m_stepSize = system.m_stepSize;

	// La copie ne peut pas (ou plutôt ne devrait pas) avoir lieu pendant une mise à jour, inutile de copier
	m_dyingParticles.clear();
	m_processing = false;
	m_stepAccumulator = 0.f;

	m_buffer.clear(); // Pour éviter une recopie lors du resize() qui ne servira pas à grand chose
	ResizeBuffer();

	// On ne copie que les particules vivantes
	std::memcpy(m_buffer.data(), system.m_buffer.data(), system.m_particleCount*m_particleSize);

	return *this;
}

void NzParticleSystem::GenerateAABB() const
{
	m_boundingVolume.MakeInfinite();
}

void NzParticleSystem::Register()
{
	m_scene->RegisterForUpdate(this);
}

void NzParticleSystem::ResizeBuffer()
{
	// Histoire de décrire un peu mieux l'erreur en cas d'échec
	try
	{
		m_buffer.resize(m_maxParticleCount*m_particleSize);
	}
	catch (const std::exception& e)
	{
		NzStringStream stream;
		stream << "Failed to allocate particle buffer (" << e.what() << ") for " << m_maxParticleCount << " particles of size " << m_particleSize;

		NazaraError(stream.ToString());
	}
}

void NzParticleSystem::Unregister()
{
	m_scene->UnregisterForUpdate(this);
}

void NzParticleSystem::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
		GenerateAABB();

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingVolume.Update(m_transformMatrix);
	m_boundingVolumeUpdated = true;
}

void NzParticleSystem::Update()
{
	float elapsedTime = m_scene->GetUpdateTime();

	// Émission
	for (NzParticleEmitter* emitter : m_emitters)
		emitter->Emit(*this, elapsedTime);

	// Mise à jour
	if (m_particleCount > 0)
	{
		NzParticleMapper mapper(m_buffer.data(), m_declaration);

		m_processing = true;

		// Pour éviter un verrouillage en cas d'exception
		NzCallOnExit onExit([this]()
		{
			m_processing = false;
		});

		if (m_fixedStepEnabled)
		{
			m_stepAccumulator += elapsedTime;
			while (m_stepAccumulator >= m_stepSize)
			{
				for (NzParticleController* controller : m_controllers)
					controller->Apply(*this, mapper, 0, m_particleCount-1, m_stepAccumulator);

				m_stepAccumulator -= m_stepSize;
			}
		}
		else
		{
			for (NzParticleController* controller : m_controllers)
				controller->Apply(*this, mapper, 0, m_particleCount-1, elapsedTime);
		}

		m_processing = false;
		onExit.Reset();

		// On tue maintenant les particules mortes durant la mise à jour
		if (m_dyingParticles.size() < m_particleCount)
		{
			// On tue les particules depuis la dernière vers la première (en terme de place), le std::set étant trié via std::greater
			// La raison est simple, étant donné que la mort d'une particule signifie le déplacement de la dernière particule du buffer,
			// sans cette solution certaines particules pourraient échapper à la mort
			for (unsigned int index : m_dyingParticles)
				KillParticle(index);
		}
		else
			KillParticles(); // Toutes les particules sont mortes, ceci est beaucoup plus rapide

		m_dyingParticles.clear();
	}
}

