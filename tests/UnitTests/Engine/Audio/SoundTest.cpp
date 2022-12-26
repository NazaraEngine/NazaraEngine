#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

std::filesystem::path GetAssetDir();

SCENARIO("Sound", "[AUDIO][SOUND]")
{
	GIVEN("A sound")
	{
		Nz::Sound sound;

		WHEN("We load our sound")
		{
			REQUIRE(sound.LoadFromFile(GetAssetDir() / "Audio/Cat.flac"));

			THEN("We can ask the informations of the file")
			{
				CHECK(sound.GetDuration() == 8192);
				CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
				CHECK_FALSE(sound.IsLooping());
				CHECK(sound.IsPlayable());
				CHECK(sound.IsSpatializationEnabled());
				CHECK(sound.GetMinDistance() == 1.f);
				CHECK(sound.GetPitch() == 1.f);
				CHECK(sound.GetPlayingOffset() == 0);
				CHECK(sound.GetPosition() == Nz::Vector3f::Zero());
				CHECK(sound.GetVelocity() == Nz::Vector3f::Zero());
				CHECK(sound.GetVolume() == 1.f);
			}

			THEN("We can play it and get the time offset")
			{
				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(0.f);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				CHECK(sound.GetPlayingOffset() >= 950);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() <= 1500);
				sound.Pause();
				Nz::UInt32 playingOffset = sound.GetPlayingOffset();
				CHECK(sound.GetStatus() == Nz::SoundStatus::Paused);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				CHECK(sound.GetStatus() == Nz::SoundStatus::Paused);
				CHECK(sound.GetPlayingOffset() == playingOffset);

				sound.SetPlayingOffset(3500);
				CHECK(sound.GetPlayingOffset() == 3500);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() >= 1650);

				AND_WHEN("We let the sound stop by itself")
				{
					REQUIRE(sound.GetDuration() == 8192);

					sound.SetPlayingOffset(8000);
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(sound.GetPlayingOffset() == 0);

					sound.SetPlayingOffset(9000);
					sound.Play();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);

					sound.Stop();
					sound.SetPlayingOffset(8000);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK(sound.GetStatus() == Nz::SoundStatus::Stopped);
					CHECK(sound.GetPlayingOffset() == 0); //< playing offset has no effect until Play()

					AND_WHEN("We enable looping")
					{
						sound.EnableLooping(true);
						CHECK(sound.IsLooping());
						sound.Play();
						CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(sound.GetPlayingOffset() >= 8000);
						std::this_thread::sleep_for(std::chrono::milliseconds(300));
						CHECK(sound.GetStatus() == Nz::SoundStatus::Playing);
						CHECK(sound.GetPlayingOffset() < 300);
					}
				}

				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(100.f);
			}
		}
	}
}
