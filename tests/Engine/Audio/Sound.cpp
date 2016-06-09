#include <Nazara/Audio/Sound.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Thread.hpp>

SCENARIO("Sound", "[AUDIO][SOUND]")
{
	GIVEN("A sound")
	{
		Nz::Sound sound;

		WHEN("We load our sound")
		{
			REQUIRE(sound.LoadFromFile("resources/Engine/Audio/Cat.flac"));

			THEN("We can ask the informations of the file")
			{
				REQUIRE(sound.GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(sound.GetDuration() >= 8000);
				REQUIRE(sound.GetStatus() == Nz::SoundStatus_Stopped);
				REQUIRE(sound.IsLooping() == false);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::SetGlobalVolume(0.f);

				sound.Play();			
				Nz::Thread::Sleep(1000);
				REQUIRE(sound.GetPlayingOffset() >= 950);
				Nz::Thread::Sleep(200);
				REQUIRE(sound.GetPlayingOffset() <= 1300);
				sound.Pause();
				REQUIRE(sound.GetStatus() == Nz::SoundStatus_Paused);

				sound.SetPlayingOffset(3500);
				REQUIRE(sound.GetPlayingOffset() >= 3500);

				Nz::Audio::SetGlobalVolume(100.f);
			}
		}
	}
}
