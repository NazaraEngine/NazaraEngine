#include <Nazara/Audio/SoundBuffer.hpp>
#include <catch2/catch.hpp>

std::filesystem::path GetResourceDir();

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	GIVEN("A sound buffer")
	{
		WHEN("We load our sound")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "Engine/Audio/Cat.flac");
			REQUIRE(soundBuffer);

			THEN("We can ask the informations of the file")
			{
				REQUIRE(soundBuffer->GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(soundBuffer->GetDuration() >= 8000);
			}
		}
	}
}
