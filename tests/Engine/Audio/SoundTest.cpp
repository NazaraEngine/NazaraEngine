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
				CHECK(sound.GetStatus() == Nz::SoundStatus::Paused);

				sound.SetPlayingOffset(3500);
				CHECK(sound.GetPlayingOffset() == 3500);

				Nz::Audio::Instance()->GetDefaultDevice()->SetGlobalVolume(100.f);
			}
		}
	}
}
