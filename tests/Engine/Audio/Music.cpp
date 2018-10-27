#include <Nazara/Audio/Music.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Thread.hpp>

SCENARIO("Music", "[AUDIO][MUSIC]")
{
	GIVEN("A music")
	{
		Nz::Music music;

		WHEN("We load our music")
		{
			REQUIRE(music.OpenFromFile("resources/Engine/Audio/The_Brabanconne.ogg"));

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
				Nz::Audio::SetGlobalVolume(0.f);

				music.Play();
				Nz::Thread::Sleep(1000);
				REQUIRE(music.GetPlayingOffset() >= 950);
				Nz::Thread::Sleep(200);
				REQUIRE(music.GetPlayingOffset() <= 1300);
				music.Pause();
				REQUIRE(music.GetStatus() == Nz::SoundStatus_Paused);

				music.SetPlayingOffset(3500);
				REQUIRE(music.GetPlayingOffset() >= 3500);

				Nz::Audio::SetGlobalVolume(100.f);
			}
		}
	}
}
