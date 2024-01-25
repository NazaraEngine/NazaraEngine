#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Music.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

std::filesystem::path GetAssetDir();

SCENARIO("Music", "[AUDIO][MUSIC]")
{
	using namespace Nz::Literals;

	GIVEN("A music")
	{
		Nz::Music music;

		WHEN("We load our music")
		{
			REQUIRE(music.OpenFromFile(GetAssetDir() / "Audio/The_Brabanconne.ogg"));

			THEN("We can ask the informations of the file")
			{
				CHECK(music.GetDuration() == 63'059'591_us); // 1 min 03 = 63s = 63000ms
				CHECK(music.GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(music.GetPlayingOffset() == 0_ms);
				CHECK(music.GetSampleCount() <= 64 * 44100 * 2); // * 2 (stereo)
				CHECK(music.GetSampleCount() >= 63 * 44100 * 2); // * 2 (stereo)
				CHECK(music.GetSampleRate() == 44100 /* Hz */);
				CHECK(music.GetStatus() == Nz::SoundStatus::Stopped);
				CHECK_FALSE(music.IsLooping());
				CHECK(music.IsSpatializationEnabled());
				CHECK(music.GetMinDistance() == 1.f);
				CHECK(music.GetPitch() == 1.f);
				CHECK(music.GetPlayingOffset() == 0_ms);
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
				Nz::Time t = music.GetPlayingOffset();
				CHECK(music.GetPlayingOffset() >= 950_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(music.GetPlayingOffset() <= 1500_ms);

				music.SeekToPlayingOffset(4200_ms);
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
				CHECK(music.GetPlayingOffset() >= 4150_ms);
				CHECK(music.GetPlayingOffset() < 4500_ms);
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);

				music.Pause();
				Nz::Time playingOffset = music.GetPlayingOffset();
				CHECK(music.GetStatus() == Nz::SoundStatus::Paused);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				CHECK(music.GetStatus() == Nz::SoundStatus::Paused);
				CHECK(music.GetPlayingOffset() == playingOffset);

				music.SeekToPlayingOffset(3500_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(music.GetPlayingOffset() == 3500_ms);

				music.Play();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
				CHECK(music.GetPlayingOffset() >= 3650_ms);

				AND_WHEN("We let the sound stop by itself")
				{
					REQUIRE(music.GetDuration() == 63'059'591_us);

					music.SeekToPlayingOffset(62900_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
					CHECK(music.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(music.GetPlayingOffset() == 0_ms);

					music.SeekToPlayingOffset(64000_ms);
					music.Play();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
					CHECK(music.GetPlayingOffset() < 100_ms);

					music.Stop();
					music.SeekToPlayingOffset(62900_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(music.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(music.GetPlayingOffset() == 0_ms); //< playing offset has no effect until Play()

					AND_WHEN("We enable looping")
					{
						music.EnableLooping(true);
						CHECK(music.IsLooping());
						music.Play();
						CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(music.GetPlayingOffset() >= 62900_ms);
						std::this_thread::sleep_for(std::chrono::milliseconds(300));
						CHECK(music.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(music.GetPlayingOffset() < 300_ms);
					}
				}
				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(100.f);
			}
		}
	}
}
