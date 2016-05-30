#include <Nazara/Graphics/ParticleSystem.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/ParticleMapper.hpp>

class TestParticleController : public Nz::ParticleController
{
	public:
		// Be aware that the interval is [startId, endId] and NOT [startId, endId)
		void Apply(Nz::ParticleSystem& system, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) override
		{
			Nz::SparsePtr<Nz::Vector3f> positionPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
			Nz::SparsePtr<Nz::Vector3f> velocityPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Velocity);
			Nz::SparsePtr<float> lifePtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);

			for (unsigned int i = startId; i <= endId; ++i)
			{
				Nz::Vector3f& particlePosition = positionPtr[i];
				Nz::Vector3f& particleVelocity = velocityPtr[i];
				float& particleLife = lifePtr[i];

				particleLife -= elapsedTime;
				if (particleLife <= 0.f)
					system.KillParticle(i);
			}
		}
};

class TestParticleEmitter : public Nz::ParticleEmitter
{
	public:
		~TestParticleEmitter() override = default;

		void Emit(Nz::ParticleSystem& system, float elapsedTime) const override
		{
			system.GenerateParticles(GetEmissionCount());
		}

	private:
		void SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const override
		{
		}
};

class TestParticleGenerator : public Nz::ParticleGenerator
{
	public:
		~TestParticleGenerator() override = default;

		// Be aware that the interval is [startId, endId] and NOT [startId, endId)
		void Generate(Nz::ParticleSystem& system, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId) override
		{
			Nz::SparsePtr<Nz::Vector3f> positionPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
			Nz::SparsePtr<Nz::Vector3f> velocityPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Velocity);
			Nz::SparsePtr<float> lifePtr = mapper.GetComponentPtr<float>(Nz::ParticleComponent_Life);

			for (unsigned int i = startId; i <= endId; ++i)
			{
				Nz::Vector3f& particlePosition = positionPtr[i];
				Nz::Vector3f& particleVelocity = velocityPtr[i];
				float& particleLife = lifePtr[i];

				particlePosition = Nz::Vector3f::Zero();
				particleVelocity = Nz::Vector3f::UnitX();
				particleLife = 1.3f;
			}
		}
};

SCENARIO("ParticleSystem", "[GRAPHICS][PARTICLESYSTEM]")
{
	GIVEN("A particle system of maximum 10 billboards with its generators")
	{
		// These need to be alive longer than the particle system
		TestParticleController particleController;
		TestParticleGenerator particleGenerator;
		Nz::ParticleSystem particleSystem(10, Nz::ParticleLayout_Billboard);

		particleSystem.AddController(&particleController);
		TestParticleEmitter particleEmitter;
		particleEmitter.SetEmissionCount(10);
		particleSystem.AddEmitter(&particleEmitter);

		particleSystem.AddGenerator(&particleGenerator);

		WHEN("We update to generate 10 particles")
		{
			particleSystem.Update(1.f);

			THEN("There must be 10 particles")
			{
				REQUIRE(particleSystem.GetParticleCount() == 10);
			}

			AND_THEN("We update to make them die")
			{
				particleSystem.Update(2.f);
				REQUIRE(particleSystem.GetParticleCount() == 0);
			}
		}
	}
}
