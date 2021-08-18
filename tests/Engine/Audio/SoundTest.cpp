#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <catch2/catch.hpp>
#include <chrono>
#include <thread>

std::filesystem::path GetResourceDir();

SCENARIO("Sound", "[AUDIO][SOUND]")
{
	GIVEN("A sound")
	{
		Nz::Sound sound;

		WHEN("We load our sound")
		{
			REQUIRE(sound.LoadFromFile(GetResourceDir() / "Engine/Audio/Cat.flac"));

			THEN("We can ask the informations of the file")
			{
				REQUIRE(sound.GetDuration() <= 8500); // 8s = 8000ms
				REQUIRE(sound.GetDuration() >= 8000);
				REQUIRE(sound.GetStatus() == Nz::SoundStatus::Stopped);
				REQUIRE(sound.IsLooping() == false);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::Instance()->SetGlobalVolume(0.f);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				REQUIRE(sound.GetPlayingOffset() >= 950);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				REQUIRE(sound.GetPlayingOffset() <= 1300);
				sound.Pause();
				REQUIRE(sound.GetStatus() == Nz::SoundStatus::Paused);

				sound.SetPlayingOffset(3500);
				REQUIRE(sound.GetPlayingOffset() >= 3500);

				Nz::Audio::Instance()->SetGlobalVolume(100.f);
			}
		}
	}
}
