#include <Nazara/Audio/SoundBuffer.hpp>
#include <Catch/catch.hpp>

std::filesystem::path GetResourceDir();

SCENARIO("SoundBuffer", "[AUDIO][SOUNDBUFFER]")
{
	GIVEN("A sound buffer")
	{
		WHEN("We load our sound")
		{
			Nz::std::shared_ptr<SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetResourceDir() / "Engine/Audio/Cat.flac");
			REQUIRE(soundBuffer.IsValid());

			THEN("We can ask the informations of the file")
			{
				REQUIRE(soundBuffer->GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(soundBuffer->GetDuration() >= 8000);
			}
		}
	}
}
