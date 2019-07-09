#include <Nazara/Audio/SoundBuffer.hpp>
#include <Catch/catch.hpp>

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	GIVEN("A sound buffer")
	{
		WHEN("We load our sound")
		{
			Nz::SoundBufferRef soundBuffer = Nz::SoundBuffer::LoadFromFile("resources/Engine/Audio/Cat.flac");
			REQUIRE(soundBuffer.IsValid());

			THEN("We can ask the informations of the file")
			{
				REQUIRE(soundBuffer->GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(soundBuffer->GetDuration() >= 8000);
			}
		}
	}
}
