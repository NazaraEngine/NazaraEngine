#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Music.hpp>
#include <catch2/catch.hpp>
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
				CHECK(music.GetDuration() == 63059); // 1 min 03 = 63s = 63000ms
				CHECK(music.GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(music.GetPlayingOffset() == 0);
				CHECK(music.GetSampleCount() <= 64 * 44100 * 2); // * 2 (stereo)
				CHECK(music.GetSampleCount() >= 63 * 44100 * 2); // * 2 (stereo)
				CHECK(music.GetSampleRate() == 44100 /* Hz */);
				CHECK(music.GetStatus() == Nz::SoundStatus::Stopped);
				CHECK_FALSE(music.IsLooping());
				CHECK(music.IsSpatializationEnabled());
				CHECK(music.GetMinDistance() == 1.f);
				CHECK(music.GetPitch() == 1.f);
				CHECK(music.GetPlayingOffset() == 0);
				CHECK(music.GetPosition() == Nz::Vector3f::Zero());
				CHECK(music.GetVelocity() == Nz::Vector3f::Zero());
				CHECK(music.GetVolume() == 1.f);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(0.f);

				music.Play();
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				CHECK(music.GetPlayingOffset() >= 950);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(music.GetPlayingOffset() <= 1500);

				music.SetPlayingOffset(4200);
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
				CHECK(music.GetPlayingOffset() >= 4150);
				CHECK(music.GetPlayingOffset() < 4500);
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);

				music.Pause();
				CHECK(music.GetStatus() == Nz::SoundStatus::Paused);

				music.SetPlayingOffset(3500);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(music.GetPlayingOffset() == 3500);

				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(100.f);
			}
		}
	}
}
