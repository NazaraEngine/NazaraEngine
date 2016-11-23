#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Catch/catch.hpp>

SCENARIO("ParticleDeclaration", "[GRAPHICS][PARTICLEDECLARATION]")
{
	GIVEN("A particle declaration of a model")
	{
		Nz::ParticleDeclaration* particleDeclaration = Nz::ParticleDeclaration::Get(Nz::ParticleLayout_Model);

		WHEN("We disable a component")
		{
			bool enabled;
			Nz::ComponentType type;
			std::size_t offset;
			particleDeclaration->GetComponent(Nz::ParticleComponent_Position, &enabled, &type, &offset);
			REQUIRE(enabled);
			std::size_t oldStride = particleDeclaration->GetStride();

			particleDeclaration->DisableComponent(Nz::ParticleComponent_Position);
			REQUIRE(oldStride != particleDeclaration->GetStride());

			THEN("We can enable it and the stride is back")
			{
				particleDeclaration->EnableComponent(Nz::ParticleComponent_Position, type, offset);
				REQUIRE(oldStride == particleDeclaration->GetStride());
			}
		}
	}
}
