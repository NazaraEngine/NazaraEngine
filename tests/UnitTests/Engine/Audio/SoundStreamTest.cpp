#include <Nazara/Audio/SoundStream.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

std::filesystem::path GetAssetDir();

SCENARIO("SoundStream", "[AUDIO][SoundStream]")
{
	using namespace Nz::Literals;

	GIVEN("A sound buffer")
	{
		WHEN("We load a .flac file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/Cat.flac");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 8192_ms);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleCount() == 1572864);
				CHECK(soundStream->GetSampleRate() == 96000);
			}
		}

		WHEN("We load a .mp3 file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/file_example_MP3_700KB.mp3");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 27'193'468_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleCount() == 1746432);
				CHECK(soundStream->GetSampleRate() == 32000);
			}
		}

		WHEN("We load a .ogg file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/The_Brabanconne.ogg");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 63'059'591_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Stereo);
				CHECK(soundStream->GetSampleCount() == 5561856);
				CHECK(soundStream->GetSampleRate() == 44100);
			}
		}

		WHEN("We load a .wav file")
		{
			std::shared_ptr<Nz::SoundStream> soundStream = Nz::SoundStream::OpenFromFile(GetAssetDir() / "Audio/explosion1.wav");
			REQUIRE(soundStream);

			THEN("We can ask the informations of the file")
			{
				CHECK(soundStream->GetDuration() == 2'490'340_us);
				CHECK(soundStream->GetFormat() == Nz::AudioFormat::I16_Mono);
				CHECK(soundStream->GetSampleCount() == 109824);
				CHECK(soundStream->GetSampleRate() == 44100);
			}
		}
	}
}
