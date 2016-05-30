#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Catch/catch.hpp>

SCENARIO("SkyboxBackground", "[GRAPHICS][SKYBOXBACKGROUND]")
{
	GIVEN("A skybox background with a loaded image")
	{
		Nz::TextureRef textureRef = Nz::Texture::New();
		textureRef->LoadCubemapFromFile("resources/Engine/Graphics/skybox.png");
		Nz::SkyboxBackgroundRef skyboxBackgroundRef = Nz::SkyboxBackground::New(textureRef);

		WHEN("We assign it parameters")
		{
			skyboxBackgroundRef->SetMovementOffset(Nz::Vector3f::Unit());
			skyboxBackgroundRef->SetMovementScale(1.f);

			THEN("We can get it")
			{
				REQUIRE(skyboxBackgroundRef->GetMovementOffset() == Nz::Vector3f::Unit());
				REQUIRE(skyboxBackgroundRef->GetMovementScale() == Approx(1.f));
				REQUIRE(skyboxBackgroundRef->GetTexture().Get() == textureRef.Get());
			}
		}
	}
}
