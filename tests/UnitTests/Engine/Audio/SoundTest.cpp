#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iostream>
#include <thread>

std::filesystem::path GetAssetDir();

SCENARIO("Sound", "[AUDIO][SOUND]")
{
	using namespace Nz::Literals;

	GIVEN("A sound")
	{
		Nz::Sound sound;

		WHEN("We load our sound")
		{
			REQUIRE(sound.LoadFromFile(GetAssetDir() / "Audio/Cat.flac"));

			THEN("We can ask the informations of the file")
			{
				CHECK(sound.GetDuration() == 8192_ms);
				CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
				CHECK_FALSE(sound.IsLooping());
				CHECK(sound.IsPlayable());
				CHECK(sound.IsSpatializationEnabled());
				CHECK(sound.GetMinDistance() == 1.f);
				CHECK(sound.GetPitch() == 1.f);
				CHECK(sound.GetPlayingOffset() == 0_ms);
				CHECK(sound.GetPosition() == Nz::Vector3f::Zero());
				CHECK(sound.GetVelocity() == Nz::Vector3f::Zero());
				CHECK(sound.GetVolume() == 1.f);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(0.f);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::seconds(1));

				CHECK(sound.GetPlayingOffset() >= 950_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() <= 1500_ms);
				sound.Pause();
				Nz::Time playingOffset = sound.GetPlayingOffset();
				Nz::UInt64 sampleOffset = sound.GetSampleOffset();
				CHECK(sound.GetStatus() == Nz::SoundStatus::Paused);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				CHECK(sound.GetStatus() == Nz::SoundStatus::Paused);
				CHECK(sound.GetPlayingOffset() == playingOffset);
				CHECK(sound.GetSampleOffset() == sampleOffset);

				sound.SeekToPlayingOffset(3500_ms);
				CHECK(sound.GetPlayingOffset() == 3500_ms);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() >= 1650_ms);

				AND_WHEN("We let the sound stop by itself")
				{
					REQUIRE(sound.GetDuration() == 8192_ms);

					sound.SeekToPlayingOffset(8000_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(sound.GetPlayingOffset() == 0_ms);

					sound.SeekToPlayingOffset(9000_ms);
					sound.Play();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);

					sound.Stop();
					sound.SeekToPlayingOffset(8000_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(sound.GetPlayingOffset() == 0_ms); //< playing offset has no effect until Play()

					AND_WHEN("We enable looping")
					{
						sound.EnableLooping(true);
						CHECK(sound.IsLooping());
						sound.Play();
						CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(sound.GetPlayingOffset() >= 8000_ms);
						std::this_thread::sleep_for(std::chrono::milliseconds(300));
						CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(sound.GetPlayingOffset() < 300_ms);
					}
				}

				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(100.f);
			}
		}
	}
}
