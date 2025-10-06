#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

std::filesystem::path GetAssetDir();

SCENARIO("SoundEmitter", "[AUDIO][SOUNDEMITTER]")
{
	GIVEN("A sound emitter")
	{
		std::shared_ptr<Nz::AudioEngine> audioEngine = Nz::Audio::Instance()->OpenPlaybackEngine();

		std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(GetAssetDir() / "Audio/Cat.flac");
		REQUIRE(soundBuffer);

		WHEN("We create a sound")
		{
			Nz::Sound sound({ .engine = audioEngine.get(), .source = soundBuffer });

			THEN("We can ask information about position and velocity")
			{
				sound.EnableSpatialization(true);
				sound.SetPosition(Nz::Vector3f::Zero());
				sound.SetVelocity(Nz::Vector3f::UnitX());

				REQUIRE(sound.IsSpatializationEnabled());
				REQUIRE(sound.GetPosition() == Nz::Vector3f::Zero());
				REQUIRE(sound.GetVelocity() == Nz::Vector3f::UnitX());
			}

			THEN("We can ask information about attenuation, pitch, ...")
			{
				sound.SetAttenuationModel(Nz::SoundAttenuationModel::Inverse);
				sound.SetMinDistance(40.f);
				sound.SetPitch(0.8f);
				sound.SetVolume(50.f);

				REQUIRE(sound.GetAttenuationModel() == Nz::SoundAttenuationModel::Inverse);
				REQUIRE(Catch::Approx(sound.GetMinDistance()) == 40.f);
				REQUIRE(Catch::Approx(sound.GetPitch()) == 0.8f);
				REQUIRE(Catch::Approx(sound.GetVolume()) == 50.f);
			}
		}
	}
}
