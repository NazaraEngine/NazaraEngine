#include <Nazara/Graphics/TextureBackground.hpp>
#include <Catch/catch.hpp>

SCENARIO("TextureBackground", "[GRAPHICS][TEXTUREBACKGROUND]")
{
	GIVEN("A default texture background")
	{
		Nz::TextureRef textureRef = Nz::Texture::New();
		textureRef->LoadFromFile("resources/Engine/Graphics/skybox.png");
		Nz::TextureBackgroundRef textureBackgroundRef = Nz::TextureBackground::New(textureRef);

		WHEN("We assign it parameters")
		{
			THEN("We can get it")
			{
				REQUIRE(textureBackgroundRef->GetTexture().Get() == textureRef.Get());
			}
		}
	}
}