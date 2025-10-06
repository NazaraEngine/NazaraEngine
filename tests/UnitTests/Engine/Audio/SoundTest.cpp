#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/AudioEngine.hpp>
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
		std::shared_ptr<Nz::AudioEngine> audioEngine = Nz::Audio::Instance()->OpenPlaybackEngine();

		WHEN("We load our sound")
		{
			std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/Cat.flac");
			REQUIRE(soundBuffer);

			Nz::Sound sound({ .engine = audioEngine.get(), .source = soundBuffer });

			THEN("We can ask the informations of the file")
			{
				CHECK(sound.GetDuration() == 8192_ms);
				CHECK_FALSE(sound.IsPlaying());
				CHECK_FALSE(sound.IsLooping());
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
				sound.Play(true);
				std::this_thread::sleep_for(std::chrono::seconds(1));

				CHECK(sound.GetPlayingOffset() >= 950_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() <= 1500_ms);
				sound.Pause(true);
				Nz::Time playingOffset = sound.GetPlayingOffset();
				Nz::UInt64 frameOffset = sound.GetPlayingFrame();
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				CHECK(sound.GetPlayingOffset() == playingOffset);
				CHECK(sound.GetPlayingFrame() == frameOffset);

				sound.SeekToTime(3500_ms);
				CHECK(sound.GetPlayingOffset() == 3500_ms);

				sound.Play();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				CHECK(sound.GetPlayingOffset() >= 1650_ms);

				AND_WHEN("We let the sound stop by itself")
				{
					REQUIRE(sound.GetDuration() == 8192_ms);

					sound.SeekToTime(8000_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					CHECK_FALSE(sound.IsPlaying());
					CHECK(sound.GetPlayingOffset() >= 8192_ms); //< >= because of Linux that gives 8.19202s for some reason

					sound.SeekToTime(9000_ms);
					sound.Play();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK_FALSE(sound.IsPlaying());

					sound.Stop();
					sound.SeekToTime(8000_ms);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CHECK_FALSE(sound.IsPlaying());
					CHECK(sound.GetPlayingOffset() == 8000_ms);

					AND_WHEN("We enable looping")
					{
						sound.EnableLooping(true);
						CHECK(sound.IsLooping());
						sound.Play();
						CHECK(sound.IsPlaying());
						CHECK(sound.GetPlayingOffset() >= 8000_ms);
						std::this_thread::sleep_for(std::chrono::milliseconds(300));
						CHECK(sound.IsPlaying());
						CHECK(sound.GetPlayingOffset() < 300_ms);
					}
				}
			}
		}
	}
}
