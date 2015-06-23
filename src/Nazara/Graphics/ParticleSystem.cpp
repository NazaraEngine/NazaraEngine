// Copyright (C) 2015 Jérôme Leclercq
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
m_processing(false),
m_maxParticleCount(maxParticleCount),
m_particleCount(0)
{
	// En cas d'erreur, un constructeur ne peut que lancer une exception
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_particleSize = m_declaration->GetStride(); // La taille de chaque particule

	ResizeBuffer();
}

NzParticleSystem::NzParticleSystem(const NzParticleSystem& system) :
NzRenderable(system),
m_controllers(system.m_controllers),
m_generators(system.m_generators),
m_declaration(system.m_declaration),
m_renderer(system.m_renderer),
m_processing(false),
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
	NazaraAssert(controller, "Invalid particle controller");

	m_controllers.emplace_back(controller);
}

void NzParticleSystem::AddEmitter(NzParticleEmitter* emitter)
{
	NazaraAssert(emitter, "Invalid particle emitter");

	m_emitters.emplace_back(emitter);
}

void NzParticleSystem::AddGenerator(NzParticleGenerator* generator)
{
	NazaraAssert(generator, "Invalid particle generator");

	m_generators.emplace_back(generator);
}

void NzParticleSystem::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const
{
	NazaraAssert(m_renderer, "Invalid particle renderer");
	NazaraAssert(renderQueue, "Invalid renderqueue");
	NazaraUnused(transformMatrix);

	if (m_particleCount > 0)
	{
		NzParticleMapper mapper(m_buffer.data(), m_declaration);
		m_renderer->Render(*this, mapper, 0, m_particleCount-1, renderQueue);
	}
}

void NzParticleSystem::ApplyControllers(NzParticleMapper& mapper, unsigned int particleCount, float elapsedTime)
{
	m_processing = true;

	// Pour éviter un verrouillage en cas d'exception
	NzCallOnExit onExit([this]()
	{
		m_processing = false;
	});

	for (NzParticleController* controller : m_controllers)
		controller->Apply(*this, mapper, 0, particleCount-1, elapsedTime);

	onExit.CallAndReset();

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

void* NzParticleSystem::CreateParticle()
{
	return CreateParticles(1);
}

void* NzParticleSystem::CreateParticles(unsigned int count)
{
	if (count == 0)
		return nullptr;

	if (m_particleCount + count > m_maxParticleCount)
		return nullptr;

	unsigned int particlesIndex = m_particleCount;
	m_particleCount += count;

	return &m_buffer[particlesIndex*m_particleSize];
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
		generator->Generate(*this, mapper, 0, count-1);

	return ptr;
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

bool NzParticleSystem::IsDrawable() const
{
	return true;
}

void NzParticleSystem::KillParticle(unsigned int index)
{
	///FIXME: Vérifier index

	if (m_processing)
	{
		// Le buffer est en train d'être modifié, nous ne pouvons pas réduire sa taille, on place alors la particule dans une liste d'attente
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

void NzParticleSystem::Update(float elapsedTime)
{
	// Émission
	for (NzParticleEmitter* emitter : m_emitters)
		emitter->Emit(*this, elapsedTime);

	// Mise à jour
	if (m_particleCount > 0)
	{
		///TODO: Mettre à jour en utilisant des threads
		NzParticleMapper mapper(m_buffer.data(), m_declaration);
		ApplyControllers(mapper, m_particleCount, elapsedTime);
	}
}

void NzParticleSystem::UpdateBoundingVolume(const NzMatrix4f& transformMatrix)
{
	NazaraUnused(transformMatrix);

	// Nothing to do here (our bounding volume is global)
}

NzParticleSystem& NzParticleSystem::operator=(const NzParticleSystem& system)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	NzRenderable::operator=(system);

	m_controllers = system.m_controllers;
	m_declaration = system.m_declaration;
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

void NzParticleSystem::MakeBoundingVolume() const
{
	///TODO: Calculer l'AABB (prendre la taille des particules en compte s'il y a)
	m_boundingVolume.MakeInfinite();
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

