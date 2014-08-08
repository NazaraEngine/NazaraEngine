// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <cstdlib>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzParticleEmitter::NzParticleEmitter(unsigned int maxParticleCount, nzParticleLayout layout) :
NzParticleEmitter(maxParticleCount, NzParticleDeclaration::Get(layout))
{
}

NzParticleEmitter::NzParticleEmitter(unsigned int maxParticleCount, NzParticleDeclaration* declaration) :
m_declaration(declaration),
m_boundingVolumeUpdated(false),
m_fixedStepEnabled(false),
m_processing(false),
m_emissionAccumulator(0.f),
m_emissionRate(0.f),
m_stepAccumulator(0.f),
m_stepSize(1.f/60.f),
m_emissionCount(1),
m_maxParticleCount(maxParticleCount),
m_particleCount(0)
{
	// En cas d'erreur, un constructeur ne peut que lancer une exception
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_particleSize = m_declaration->GetStride(); // La taille de chaque particule

	ResizeBuffer();
}

NzParticleEmitter::NzParticleEmitter(const NzParticleEmitter& emitter) :
NzSceneNode(emitter),
m_controllers(emitter.m_controllers),
m_generators(emitter.m_generators),
m_boundingVolume(emitter.m_boundingVolume),
m_declaration(emitter.m_declaration),
m_renderer(emitter.m_renderer),
m_boundingVolumeUpdated(emitter.m_boundingVolumeUpdated),
m_fixedStepEnabled(emitter.m_fixedStepEnabled),
m_processing(false),
m_emissionAccumulator(0.f),
m_emissionRate(emitter.m_emissionRate),
m_stepAccumulator(0.f),
m_stepSize(emitter.m_stepSize),
m_emissionCount(emitter.m_emissionCount),
m_maxParticleCount(emitter.m_maxParticleCount),
m_particleCount(emitter.m_particleCount),
m_particleSize(emitter.m_particleSize)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	ResizeBuffer();

	// On ne copie que les particules vivantes
	std::memcpy(m_buffer.data(), emitter.m_buffer.data(), emitter.m_particleCount*m_particleSize);
}

NzParticleEmitter::~NzParticleEmitter() = default;

void NzParticleEmitter::AddController(NzParticleController* controller)
{
	m_controllers.emplace_back(controller);
}

void NzParticleEmitter::AddGenerator(NzParticleGenerator* generator)
{
	m_generators.emplace_back(generator);
}

void NzParticleEmitter::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	///FIXME: Vérifier le renderer
	if (m_particleCount > 0)
	{
		NzParticleMapper mapper(m_buffer.data(), m_declaration);
		m_renderer->Render(*this, mapper, 0, m_particleCount-1, renderQueue);
	}
}

void* NzParticleEmitter::CreateParticle()
{
	return CreateParticles(1);
}

void* NzParticleEmitter::CreateParticles(unsigned int count)
{
	if (m_particleCount+count > m_maxParticleCount)
		return nullptr;

	unsigned int particlesIndex = m_particleCount;
	m_particleCount += count;

	return &m_buffer[particlesIndex*m_particleSize];
}

void NzParticleEmitter::EnableFixedStep(bool fixedStep)
{
	// On teste pour empêcher que cette méthode ne remette systématiquement le step accumulator à zéro
	if (m_fixedStepEnabled != fixedStep)
	{
		m_fixedStepEnabled = fixedStep;
		m_stepAccumulator = 0.f;
	}
}

void* NzParticleEmitter::GenerateParticle()
{
	return GenerateParticles(1);
}

void* NzParticleEmitter::GenerateParticles(unsigned int count)
{
	void* ptr = CreateParticles(count);
	if (!ptr)
		return nullptr;

	NzParticleMapper mapper(ptr, m_declaration);
	for (NzParticleGenerator* generator : m_generators)
		generator->Generate(*this, mapper, 0, m_particleCount-1);

	return ptr;
}

const NzBoundingVolumef& NzParticleEmitter::GetBoundingVolume() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return m_boundingVolume;
}

unsigned int NzParticleEmitter::GetEmissionCount() const
{
	return m_emissionCount;
}

float NzParticleEmitter::GetEmissionRate() const
{
	return m_emissionRate;
}

unsigned int NzParticleEmitter::GetMaxParticleCount() const
{
	return m_maxParticleCount;
}

unsigned int NzParticleEmitter::GetParticleCount() const
{
	return m_particleCount;
}

unsigned int NzParticleEmitter::GetParticleSize() const
{
	return m_particleSize;
}

nzSceneNodeType NzParticleEmitter::GetSceneNodeType() const
{
	return nzSceneNodeType_ParticleEmitter;
}

bool NzParticleEmitter::IsDrawable() const
{
	return true;
}

bool NzParticleEmitter::IsFixedStepEnabled() const
{
	return m_fixedStepEnabled;
}

void NzParticleEmitter::KillParticle(unsigned int index)
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

void NzParticleEmitter::KillParticles()
{
	m_particleCount = 0;
}

void NzParticleEmitter::RemoveController(NzParticleController* controller)
{
	auto it = std::find(m_controllers.begin(), m_controllers.end(), controller);
	if (it != m_controllers.end())
		m_controllers.erase(it);
}

void NzParticleEmitter::RemoveGenerator(NzParticleGenerator* generator)
{
	auto it = std::find(m_generators.begin(), m_generators.end(), generator);
	if (it != m_generators.end())
		m_generators.erase(it);
}

void NzParticleEmitter::SetEmissionCount(unsigned int count)
{
	m_emissionCount = count;
}

void NzParticleEmitter::SetEmissionRate(float rate)
{
	m_emissionRate = rate;
}

void NzParticleEmitter::SetRenderer(NzParticleRenderer* renderer)
{
	m_renderer = renderer;
}

NzParticleEmitter& NzParticleEmitter::operator=(const NzParticleEmitter& emitter)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	NzSceneNode::operator=(emitter);

	m_boundingVolume = emitter.m_boundingVolume;
	m_boundingVolumeUpdated = emitter.m_boundingVolumeUpdated;
	m_controllers = emitter.m_controllers;
	m_declaration = emitter.m_declaration;
	m_emissionCount = emitter.m_emissionCount;
	m_emissionRate = emitter.m_emissionRate;
	m_fixedStepEnabled = emitter.m_fixedStepEnabled;
	m_generators = emitter.m_generators;
	m_maxParticleCount = emitter.m_maxParticleCount;
	m_particleCount = emitter.m_particleCount;
	m_particleSize = emitter.m_particleSize;
	m_renderer = emitter.m_renderer;
	m_stepSize = emitter.m_stepSize;

	// La copie ne peut pas (ou plutôt ne devrait pas) avoir lieu pendant une mise à jour, inutile de copier
	m_dyingParticles.clear();
	m_emissionAccumulator = 0.f;
	m_processing = false;
	m_stepAccumulator = 0.f;

	m_buffer.clear(); // Pour éviter une recopie lors du resize() qui ne servira pas à grand chose
	ResizeBuffer();

	// On ne copie que les particules vivantes
	std::memcpy(m_buffer.data(), emitter.m_buffer.data(), emitter.m_particleCount*m_particleSize);

	return *this;
}

void NzParticleEmitter::GenerateAABB() const
{
	m_boundingVolume.MakeInfinite();
}

void NzParticleEmitter::Register()
{
	m_scene->RegisterForUpdate(this);
}

void NzParticleEmitter::ResizeBuffer()
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

void NzParticleEmitter::Unregister()
{
	m_scene->UnregisterForUpdate(this);
}

void NzParticleEmitter::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
		GenerateAABB();

	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	m_boundingVolume.Update(m_transformMatrix);
	m_boundingVolumeUpdated = true;
}

void NzParticleEmitter::Update()
{
	float elapsedTime = m_scene->GetUpdateTime();

	if (m_emissionRate > 0.f)
	{
		// On accumule la partie réelle (pour éviter qu'un taux d'update élevé empêche des particules de se former)
		m_emissionAccumulator += elapsedTime*m_emissionRate;

		float emissionCount = std::floor(m_emissionAccumulator); // Le nombre d'émissions de cette mise à jour
		m_emissionAccumulator -= emissionCount; // On enlève la partie entière

		if (emissionCount >= 1.f)
		{
			// On calcule le nombre maximum de particules pouvant être émises cette fois-ci
			unsigned int maxParticleCount = static_cast<unsigned int>(emissionCount)*m_emissionCount;

			// On récupère le nombre de particules qu'il est possible de créer selon l'espace libre
			unsigned int particleCount = std::min(maxParticleCount, m_maxParticleCount - m_particleCount);

			// Et on émet nos particules
			GenerateParticles(particleCount);
		}
	}

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
