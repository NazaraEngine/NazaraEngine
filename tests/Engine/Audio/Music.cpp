#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Music.hpp>
#include <Catch/catch.hpp>
#include <chrono>
#include <thread>

std::filesystem::path GetResourceDir();

SCENARIO("Music", "[AUDIO][MUSIC]")
{
	GIVEN("A music")
	{
		Nz::Music music;

		WHEN("We load our music")
		{
			REQUIRE(music.OpenFromFile(GetResourceDir() / "Engine/Audio/The_Brabanconne.ogg"));

			THEN("We can ask the informations of the file")
			{
				CHECK(music.GetDuration() <= 64000); // 1 min 03 = 63s = 63000ms
				CHECK(music.GetDuration() >= 63000);
				CHECK(music.GetFormat() == Nz::AudioFormat_Stereo);
				CHECK(music.GetPlayingOffset() == 0);
				CHECK(music.GetSampleCount() <= 5644800); // 64s * 44100 Hz * 2 (stereo)
				CHECK(music.GetSampleCount() >= 5556600); // 63s * 44100 Hz * 2 (stereo)
				CHECK(music.GetSampleRate() == 44100 /* Hz */);
				CHECK(music.GetStatus() == Nz::SoundStatus_Stopped);
				CHECK(music.IsLooping() == false);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::Instance()->SetGlobalVolume(0.f);

				music.Play();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				REQUIRE(music.GetPlayingOffset() >= 950);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				REQUIRE(music.GetPlayingOffset() <= 1300);
				music.Pause();
				REQUIRE(music.GetStatus() == Nz::SoundStatus_Paused);

				music.SetPlayingOffset(3500);
				REQUIRE(music.GetPlayingOffset() >= 3500);

				Nz::Audio::Instance()->SetGlobalVolume(100.f);
			}
		}
	}
}
