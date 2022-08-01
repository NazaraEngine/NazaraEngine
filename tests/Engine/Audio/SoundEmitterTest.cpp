#include <Nazara/Audio/Sound.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

std::filesystem::path GetAssetDir();

SCENARIO("SoundEmitter", "[AUDIO][SOUNDEMITTER]")
{
	GIVEN("A sound emitter")
	{
		Nz::Sound sound;

		WHEN("We load our sound")
		{
			REQUIRE(sound.LoadFromFile(GetAssetDir() / "Audio/Cat.flac"));

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
				sound.SetAttenuation(0.4f);
				sound.SetMinDistance(40.f);
				sound.SetPitch(0.8f);
				sound.SetVolume(50.f);

				REQUIRE(Catch::Approx(sound.GetAttenuation()) == 0.4f);
				REQUIRE(Catch::Approx(sound.GetMinDistance()) == 40.f);
				REQUIRE(Catch::Approx(sound.GetPitch()) == 0.8f);
				REQUIRE(Catch::Approx(sound.GetVolume()) == 50.f);
			}
		}
	}
}
