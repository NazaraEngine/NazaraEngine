// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleGroup.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleSystem
	* \brief Graphics class that represents the system to handle particles
	*/

	/*!
	* \brief Constructs a ParticleSystem object with a maximal number of particles and a layout
	*
	* \param maxParticleCount Maximum number of particles to generate
	* \param layout Enumeration for the layout of data information for the particles
	*/

	ParticleGroup::ParticleGroup(unsigned int maxParticleCount, ParticleLayout layout) :
	ParticleGroup(maxParticleCount, ParticleDeclaration::Get(layout))
	{
	}

	/*!
	* \brief Constructs a ParticleSystem object with a maximal number of particles and a particle declaration
	*
	* \param maxParticleCount Maximum number of particles to generate
	* \param declaration Data information for the particles
	*/

	ParticleGroup::ParticleGroup(unsigned int maxParticleCount, ParticleDeclarationConstRef declaration) :
	m_maxParticleCount(maxParticleCount),
	m_particleCount(0),
	m_declaration(std::move(declaration)),
	m_processing(false)
	{
		// In case of error, the constructor can only throw an exception
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		m_particleSize = m_declaration->GetStride(); // The size of each particle

		ResizeBuffer();
	}

	/*!
	* \brief Constructs a ParticleSystem object by assignation
	*
	* \param system ParticleSystem to copy into this
	*/

	ParticleGroup::ParticleGroup(const ParticleGroup& system) :
	Renderable(system),
	m_maxParticleCount(system.m_maxParticleCount),
	m_particleCount(system.m_particleCount),
	m_particleSize(system.m_particleSize),
	m_controllers(system.m_controllers),
	m_generators(system.m_generators),
	m_declaration(system.m_declaration),
	m_renderer(system.m_renderer),
	m_processing(false)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		ResizeBuffer();

		// We only copy alive particles
		std::memcpy(m_buffer.data(), system.m_buffer.data(), system.m_particleCount*m_particleSize);
	}

	ParticleGroup::~ParticleGroup()
	{
		OnParticleGroupRelease(this);
	}

	/*!
	* \brief Adds a controller to the particles
	*
	* \param controller Controller for the particles
	*
	* \remark Produces a NazaraAssert if controller is invalid
	*/

	void ParticleGroup::AddController(ParticleControllerRef controller)
	{
		NazaraAssert(controller, "Invalid particle controller");

		m_controllers.emplace_back(std::move(controller));
	}

	/*!
	* \brief Adds an emitter to the particles
	*
	* \param emitter Emitter for the particles
	*
	* \remark Produces a NazaraAssert if emitter is invalid
	*/

	void ParticleGroup::AddEmitter(ParticleEmitter* emitter)
	{
		NazaraAssert(emitter, "Invalid particle emitter");

		EmitterEntry entry;
		entry.emitter = emitter;
		entry.moveSlot.Connect(emitter->OnParticleEmitterMove, this, &ParticleGroup::OnEmitterMove);
		entry.releaseSlot.Connect(emitter->OnParticleEmitterRelease, this, &ParticleGroup::OnEmitterRelease);

		m_emitters.emplace_back(std::move(entry));
	}

	/*!
	* \brief Adds a generator to the particles
	*
	* \param generator Generator for the particles
	*
	* \remark Produces a NazaraAssert if generator is invalid
	*/

	void ParticleGroup::AddGenerator(ParticleGeneratorRef generator)
	{
		NazaraAssert(generator, "Invalid particle generator");

		m_generators.emplace_back(std::move(generator));
	}

	/*!
	* \brief Adds the particle system to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param transformMatrix Transformation matrix for the system
	*
	* \remark Produces a NazaraAssert if inner renderer is invalid
	* \remark Produces a NazaraAssert if renderQueue is invalid
	*/

	void ParticleGroup::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& /*transformMatrix*/) const
	{
		NazaraAssert(m_renderer, "Invalid particle renderer");
		NazaraAssert(renderQueue, "Invalid renderqueue");

		if (m_particleCount > 0)
		{
			ParticleMapper mapper(m_buffer.data(), m_declaration);
			m_renderer->Render(*this, mapper, 0, m_particleCount - 1, renderQueue);
		}
	}

	/*!
	* \brief Applies the controllers
	*
	* \param mapper Mapper containing layout information of each particle
	* \param particleCount Number of particles
	* \param elapsedTime Delta time between the previous frame
	*/
	void ParticleGroup::ApplyControllers(ParticleMapper& mapper, unsigned int particleCount, float elapsedTime)
	{
		m_processing = true;

		// To avoid a lock in case of exception
		CallOnExit onExit([this]()
		{
			m_processing = false;
		});

		for (ParticleController* controller : m_controllers)
			controller->Apply(*this, mapper, 0, particleCount - 1, elapsedTime);

		onExit.CallAndReset();

		// We only kill now the dead particles during the update
		if (m_dyingParticles.size() < m_particleCount)
		{
			// We kill them in reverse order, std::set sorting them via std::greater
			// The reason is simple, as the death of a particle means moving the last particle in the buffer,
			// without this solution, certain particles could avoid death
			for (unsigned int index : m_dyingParticles)
				KillParticle(index);
		}
		else
			KillParticles(); // Every particles are dead, this is way faster

		m_dyingParticles.clear();
	}

	/*!
	* \brief Creates one particle
	* \return Pointer to the particle memory buffer
	*/

	void* ParticleGroup::CreateParticle()
	{
		return CreateParticles(1);
	}

	/*!
	* \brief Creates multiple particles
	* \return Pointer to the first particle memory buffer
	*/

	void* ParticleGroup::CreateParticles(unsigned int count)
	{
		if (count == 0)
			return nullptr;

		if (m_particleCount + count > m_maxParticleCount)
			return nullptr;

		std::size_t particlesIndex = m_particleCount;
		m_particleCount += count;

		return &m_buffer[particlesIndex * m_particleSize];
	}

	/*!
	* \brief Generates one particle
	* \return Pointer to the particle memory buffer
	*/

	void* ParticleGroup::GenerateParticle()
	{
		return GenerateParticles(1);
	}

	/*!
	* \brief Generates multiple particles
	* \return Pointer to the first particle memory buffer
	*/

	void* ParticleGroup::GenerateParticles(unsigned int count)
	{
		void* ptr = CreateParticles(count);
		if (!ptr)
			return nullptr;

		ParticleMapper mapper(ptr, m_declaration);
		for (ParticleGenerator* generator : m_generators)
			generator->Generate(*this, mapper, 0, count - 1);

		return ptr;
	}

	/*!
	* \brief Gets the particle declaration
	* \return Particle declaration
	*/

	const ParticleDeclarationConstRef& ParticleGroup::GetDeclaration() const
	{
		return m_declaration;
	}

	/*!
	* \brief Gets the maximum number of particles
	* \return Current maximum number
	*/

	std::size_t ParticleGroup::GetMaxParticleCount() const
	{
		return m_maxParticleCount;
	}

	/*!
	* \brief Gets the number of particles
	* \return Current number
	*/

	std::size_t ParticleGroup::GetParticleCount() const
	{
		return m_particleCount;
	}

	/*!
	* \brief Gets the size of particles
	* \return Current size
	*/

	std::size_t ParticleGroup::GetParticleSize() const
	{
		return m_particleSize;
	}

	/*!
	* \brief Kills one particle
	*
	* \param index Index of the particle
	*/

	void ParticleGroup::KillParticle(std::size_t index)
	{
		///FIXME: Verify the index

		if (m_processing)
		{
			// The buffer is being modified, we can not reduce its size, we put the particle in the waiting list
			m_dyingParticles.insert(index);
			return;
		}

		// We move the last alive particle to the place of this one
		if (--m_particleCount > 0)
			std::memcpy(&m_buffer[index * m_particleSize], &m_buffer[m_particleCount * m_particleSize], m_particleSize);
	}

	/*!
	* \brief Kills every particles
	*/

	void ParticleGroup::KillParticles()
	{
		m_particleCount = 0;
	}

	/*!
	* \brief Removes a controller to the particles
	*
	* \param controller Controller for the particles to remove
	*/

	void ParticleGroup::RemoveController(ParticleController* controller)
	{
		auto it = std::find(m_controllers.begin(), m_controllers.end(), controller);
		if (it != m_controllers.end())
			m_controllers.erase(it);
	}

	/*!
	* \brief Removes an emitter to the particles
	*
	* \param emitter Emitter for the particles to remove
	*/

	void ParticleGroup::RemoveEmitter(ParticleEmitter* emitter)
	{
		for (auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
		{
			if (it->emitter == emitter)
			{
				m_emitters.erase(it);
				break;
			}
		}
	}

	/*!
	* \brief Removes a generator to the particles
	*
	* \param generator Generator for the particles to remove
	*/

	void ParticleGroup::RemoveGenerator(ParticleGenerator* generator)
	{
		auto it = std::find(m_generators.begin(), m_generators.end(), generator);
		if (it != m_generators.end())
			m_generators.erase(it);
	}

	/*!
	* \brief Sets the renderer of the particles
	*
	* \param renderer Renderer for the particles
	*/

	void ParticleGroup::SetRenderer(ParticleRenderer* renderer)
	{
		m_renderer = renderer;
	}

	/*!
	* \brief Updates the system
	*
	* \param elapsedTime Delta time between the previous frame
	*/

	void ParticleGroup::Update(float elapsedTime)
	{
		// Emission
		for (const EmitterEntry& entry : m_emitters)
			entry.emitter->Emit(*this, elapsedTime);

		// Update
		if (m_particleCount > 0)
		{
			///TODO: Update using threads
			ParticleMapper mapper(m_buffer.data(), m_declaration);
			ApplyControllers(mapper, m_particleCount, elapsedTime);
		}
	}

	/*!
	* \brief Updates the bounding volume by a matrix
	*
	* \param transformMatrix Matrix transformation for our bounding volume
	*/

	void ParticleGroup::UpdateBoundingVolume(const Matrix4f& /*transformMatrix*/)
	{
		// Nothing to do here (our bounding volume is global)
	}

	/*!
	* \brief Sets the current particle system with the content of the other one
	* \return A reference to this
	*
	* \param system The other ParticleSystem
	*/

	ParticleGroup& ParticleGroup::operator=(const ParticleGroup& system)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		Renderable::operator=(system);

		m_controllers = system.m_controllers;
		m_declaration = system.m_declaration;
		m_generators = system.m_generators;
		m_maxParticleCount = system.m_maxParticleCount;
		m_particleCount = system.m_particleCount;
		m_particleSize = system.m_particleSize;
		m_renderer = system.m_renderer;

		// The copy can not (or should not) happen during the update, there is no use to copy
		m_dyingParticles.clear();
		m_processing = false;

		m_buffer.clear(); // To avoid a copy due to resize() which will be pointless
		ResizeBuffer();

		// We only copy alive particles
		std::memcpy(m_buffer.data(), system.m_buffer.data(), system.m_particleCount * m_particleSize);

		return *this;
	}

	/*!
	* \brief Makes the bounding volume of this text
	*/

	void ParticleGroup::MakeBoundingVolume() const
	{
		///TODO: Compute the AABB (taking into account the size of particles)
		m_boundingVolume.MakeInfinite();
	}

	void ParticleGroup::OnEmitterMove(ParticleEmitter* oldEmitter, ParticleEmitter* newEmitter)
	{
		for (EmitterEntry& entry : m_emitters)
		{
			if (entry.emitter == oldEmitter)
				entry.emitter = newEmitter;
		}
	}

	void ParticleGroup::OnEmitterRelease(const ParticleEmitter* emitter)
	{
		for (auto it = m_emitters.begin(); it != m_emitters.end();)
		{
			if (it->emitter == emitter)
				it = m_emitters.erase(it);
			else
				++it;
		}
	}

	/*!
	* \brief Resizes the internal buffer
	*
	* \remark Produces a NazaraError if resize did not work
	*/

	void ParticleGroup::ResizeBuffer()
	{
		// Just to have a better description of our problem in case of error
		try
		{
			m_buffer.resize(m_maxParticleCount*m_particleSize);
		}
		catch (const std::exception& e)
		{
			StringStream stream;
			stream << "Failed to allocate particle buffer (" << e.what() << ") for " << m_maxParticleCount << " particles of size " << m_particleSize;

			NazaraError(stream.ToString());
		}
	}
}
