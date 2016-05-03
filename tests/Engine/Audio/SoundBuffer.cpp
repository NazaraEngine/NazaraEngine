#include <Nazara/Audio/SoundBuffer.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Audio/Audio.hpp>

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	Nz::Initializer<Nz::Audio> audio;
	REQUIRE(audio);

	GIVEN("A sound buffer")
	{
		Nz::SoundBuffer soundBuffer;

		WHEN("We load our sound")
		{
			REQUIRE(soundBuffer.LoadFromFile("resources/Engine/Audio/Cat.flac"));

			THEN("We can ask the informations of the file")
			{
				REQUIRE(soundBuffer.GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(soundBuffer.GetDuration() >= 8000);
			}
		}
	}
}
